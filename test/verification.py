import os
import time
import unittest
import yaml

import arbitration_graphs_py_with_verifier as ag_verifier


class TestCommandVerification(unittest.TestCase):
    def setUp(self):
        self.test_behavior_high_priority = ag_verifier.testing_types.DummyBehavior(
            False, False, "HighPriority"
        )
        self.test_behavior_mid_priority = ag_verifier.testing_types.DummyBehavior(
            True, False, "MidPriority"
        )
        self.test_behavior_low_priority = ag_verifier.testing_types.DummyBehavior(
            True, True, "LowPriority"
        )

        self.time = time.time()

    def test_dummy_verifier_in_priority_arbitrator(self):
        test_priority_arbitrator = ag_verifier.PriorityArbitrator(
            "PriorityArbitrator", ag_verifier.testing_types.DummyVerifier("MidPriority")
        )

        test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag_verifier.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag_verifier.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_mid_priority,
            ag_verifier.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_low_priority,
            ag_verifier.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        self.assertTrue(test_priority_arbitrator.check_invocation_condition(self.time))

        test_priority_arbitrator.gain_control(self.time)

        self.assertEqual("LowPriority", test_priority_arbitrator.get_command(self.time))
        self.assertFalse(
            test_priority_arbitrator.options()[0].verification_result.cached(self.time)
        )
        self.assertFalse(
            test_priority_arbitrator.options()[1].verification_result.cached(self.time)
        )
        self.assertTrue(
            test_priority_arbitrator.options()[2].verification_result.cached(self.time)
        )
        self.assertTrue(
            test_priority_arbitrator.options()[3].verification_result.cached(self.time)
        )

        self.assertFalse(
            test_priority_arbitrator.options()[2]
            .verification_result.cached(self.time)
            .is_ok()
        )
        self.assertTrue(
            test_priority_arbitrator.options()[3]
            .verification_result.cached(self.time)
            .is_ok()
        )

        # Print verification results
        print(
            f"Verification result for {test_priority_arbitrator.options()[2].behavior.name}: "
            f"{test_priority_arbitrator.options()[2].verification_result.cached(self.time)}"
        )
        print(
            f"Verification result for {test_priority_arbitrator.options()[3].behavior.name}: "
            f"{test_priority_arbitrator.options()[3].verification_result.cached(self.time)}"
        )

        # fmt:off
        ps = ag_verifier.testing_types.print_strings
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "PriorityArbitrator\n"
            "    1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    2. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    3. " + ps.strike_through_on
                      + ps.invocation_true + ps.commitment_false + "MidPriority"
                      + ps.strike_through_off + "\n"
            " -> 4. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt:on
        actual_printout = test_priority_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        yaml_node = yaml.safe_load(test_priority_arbitrator.to_yaml_as_str(self.time))
        self.assertFalse("verificationResult" in yaml_node["options"][0])
        self.assertFalse("verificationResult" in yaml_node["options"][1])
        self.assertTrue("verificationResult" in yaml_node["options"][2])
        self.assertTrue("verificationResult" in yaml_node["options"][3])

        self.assertEqual("failed", yaml_node["options"][2]["verificationResult"])
        self.assertEqual("passed", yaml_node["options"][3]["verificationResult"])

        test_priority_arbitrator.lose_control(self.time)

        self.test_behavior_low_priority.invocation_condition = False
        self.assertTrue(test_priority_arbitrator.check_invocation_condition(self.time))

        test_priority_arbitrator.gain_control(self.time)

        with self.assertRaises(ag_verifier.NoApplicableOptionPassedVerificationError):
            test_priority_arbitrator.get_command(self.time)

    def test_dummy_verifier_in_priority_arbitrator_with_fallback(self):
        NO_FLAGS = ag_verifier.PriorityArbitrator.Option.Flags.NO_FLAGS
        FALLBACK = ag_verifier.PriorityArbitrator.Option.Flags.FALLBACK

        test_priority_arbitrator = ag_verifier.PriorityArbitrator(
            "PriorityArbitrator", ag_verifier.testing_types.DummyVerifier("MidPriority")
        )

        test_priority_arbitrator.add_option(self.test_behavior_high_priority, NO_FLAGS)
        test_priority_arbitrator.add_option(self.test_behavior_high_priority, NO_FLAGS)
        test_priority_arbitrator.add_option(self.test_behavior_mid_priority, NO_FLAGS)
        test_priority_arbitrator.add_option(self.test_behavior_mid_priority, FALLBACK)
        test_priority_arbitrator.add_option(self.test_behavior_low_priority, NO_FLAGS)

        self.assertTrue(test_priority_arbitrator.check_invocation_condition(self.time))

        test_priority_arbitrator.gain_control(self.time)

        self.assertEqual("MidPriority", test_priority_arbitrator.get_command(self.time))
        self.assertFalse(
            test_priority_arbitrator.options()[0].verification_result.cached(self.time)
        )
        self.assertFalse(
            test_priority_arbitrator.options()[1].verification_result.cached(self.time)
        )
        self.assertTrue(
            test_priority_arbitrator.options()[2].verification_result.cached(self.time)
        )
        self.assertTrue(
            test_priority_arbitrator.options()[3].verification_result.cached(self.time)
        )
        self.assertFalse(
            test_priority_arbitrator.options()[4].verification_result.cached(self.time)
        )

        self.assertFalse(
            test_priority_arbitrator.options()[2]
            .verification_result.cached(self.time)
            .is_ok()
        )
        self.assertFalse(
            test_priority_arbitrator.options()[3]
            .verification_result.cached(self.time)
            .is_ok()
        )

        print(
            f"verificationResult for {test_priority_arbitrator.options()[2].behavior.name}: "
            f"{test_priority_arbitrator.options()[2].verification_result.cached(self.time)}"
        )
        print(
            f"verificationResult for {test_priority_arbitrator.options()[3].behavior.name}: "
            f"{test_priority_arbitrator.options()[3].verification_result.cached(self.time)}"
        )

        # fmt:off
        ps = ag_verifier.testing_types.print_strings
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "PriorityArbitrator\n"
            "    1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    2. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    3. " + ps.strike_through_on
                      + ps.invocation_true + ps.commitment_false + "MidPriority"
                      + ps.strike_through_off + "\n"
            " -> 4. " + ps.strike_through_on
                      + ps.invocation_true + ps.commitment_false + "MidPriority"
                      + ps.strike_through_off + "\n"
            "    5. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt:on
        actual_printout = test_priority_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        yaml_node = yaml.safe_load(test_priority_arbitrator.to_yaml_as_str(self.time))
        self.assertFalse("verificationResult" in yaml_node["options"][0])
        self.assertFalse("verificationResult" in yaml_node["options"][1])
        self.assertTrue("verificationResult" in yaml_node["options"][2])
        self.assertTrue("verificationResult" in yaml_node["options"][3])
        self.assertFalse("verificationResult" in yaml_node["options"][4])

        self.assertEqual("failed", yaml_node["options"][2]["verificationResult"])
        self.assertEqual("failed", yaml_node["options"][3]["verificationResult"])

    def test_dummy_verifier_in_cost_arbitrator(self):
        NO_FLAGS = ag_verifier.CostArbitrator.Option.Flags.NO_FLAGS

        test_cost_arbitrator = ag_verifier.CostArbitrator(
            "CostArbitrator", ag_verifier.testing_types.DummyVerifier("MidPriority")
        )

        cost_map = {"HighPriority": 0, "MidPriority": 0.5, "LowPriority": 1}
        cost_estimator = ag_verifier.testing_types.CostEstimatorFromCostMap(cost_map)

        test_cost_arbitrator.add_option(
            self.test_behavior_high_priority, NO_FLAGS, cost_estimator
        )
        test_cost_arbitrator.add_option(
            self.test_behavior_high_priority, NO_FLAGS, cost_estimator
        )
        test_cost_arbitrator.add_option(
            self.test_behavior_mid_priority, NO_FLAGS, cost_estimator
        )
        test_cost_arbitrator.add_option(
            self.test_behavior_low_priority, NO_FLAGS, cost_estimator
        )

        self.assertTrue(test_cost_arbitrator.check_invocation_condition(self.time))

        test_cost_arbitrator.gain_control(self.time)

        self.assertEqual("LowPriority", test_cost_arbitrator.get_command(self.time))
        self.assertFalse(
            test_cost_arbitrator.options()[0].verification_result.cached(self.time)
        )
        self.assertFalse(
            test_cost_arbitrator.options()[1].verification_result.cached(self.time)
        )
        self.assertTrue(
            test_cost_arbitrator.options()[2].verification_result.cached(self.time)
        )
        self.assertTrue(
            test_cost_arbitrator.options()[3].verification_result.cached(self.time)
        )

        self.assertFalse(
            test_cost_arbitrator.options()[2]
            .verification_result.cached(self.time)
            .is_ok()
        )
        self.assertTrue(
            test_cost_arbitrator.options()[3]
            .verification_result.cached(self.time)
            .is_ok()
        )

        # fmt:off
        ps = ag_verifier.testing_types.print_strings
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "CostArbitrator\n"
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    - (cost: 0.500) " + ps.strike_through_on
                                   + ps.invocation_true + ps.commitment_false + "MidPriority"
                                   + ps.strike_through_off + "\n"
            " -> - (cost: 1.000) " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt:on
        actual_printout = test_cost_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        test_cost_arbitrator.lose_control(self.time)

        self.test_behavior_low_priority.invocation_condition = False
        self.assertTrue(test_cost_arbitrator.check_invocation_condition(self.time))

        test_cost_arbitrator.gain_control(self.time)

        with self.assertRaises(ag_verifier.NoApplicableOptionPassedVerificationError):
            test_cost_arbitrator.get_command(self.time)


class TestCommandVerificationInConjunctiveCoordinator(unittest.TestCase):
    def setUp(self):
        self.test_behavior_a = ag_verifier.testing_types.DummyBehavior(
            False, False, "A"
        )
        self.test_behavior_b1 = ag_verifier.testing_types.DummyBehavior(
            True, False, "B"
        )
        self.test_behavior_c = ag_verifier.testing_types.DummyBehavior(True, True, "C")
        self.test_behavior_b2 = ag_verifier.testing_types.DummyBehavior(
            True, False, "B"
        )

        self.time = time.time()

    def test_verification(self):

        NO_FLAGS = ag_verifier.ConjunctiveCoordinator.Option.Flags.NO_FLAGS

        verifying_conjunctive_coordinator = ag_verifier.ConjunctiveCoordinator(
            "ConjunctiveCoordinator", ag_verifier.testing_types.DummyVerifier("B")
        )

        verifying_conjunctive_coordinator.add_option(self.test_behavior_a, NO_FLAGS)
        verifying_conjunctive_coordinator.add_option(self.test_behavior_b1, NO_FLAGS)
        verifying_conjunctive_coordinator.add_option(self.test_behavior_c, NO_FLAGS)
        verifying_conjunctive_coordinator.add_option(self.test_behavior_b2, NO_FLAGS)

        self.test_behavior_a.invocation_condition = True

        self.assertTrue(
            verifying_conjunctive_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            verifying_conjunctive_coordinator.check_commitment_condition(self.time)
        )

        verifying_conjunctive_coordinator.gain_control(self.time)

        with self.assertRaises(ag_verifier.ApplicableOptionFailedVerificationError):
            verifying_conjunctive_coordinator.get_command(self.time)


if __name__ == "__main__":
    header = "Running " + os.path.basename(__file__)

    print("=" * len(header))
    print(header)
    print("=" * len(header) + "\n")
    unittest.main(exit=False)
    print("=" * len(header) + "\n")
