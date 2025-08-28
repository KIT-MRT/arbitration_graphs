# pyright: reportUninitializedInstanceVariable=false

import time
import unittest
from typing import cast, final

from typing_extensions import override

import arbitration_graphs as ag
from arbitration_graphs.verification import Result

from .cost_estimator import CostEstimatorFromCostMap
from .dummy_types import (
    DummyBehavior,
    DummyCommand,
    DummyEnvironmentModel,
    DummyVerifier,
    PrintStrings,
)


@final
class TestCommandVerification(unittest.TestCase):
    @override
    def setUp(self):
        self.test_behavior_high_priority = DummyBehavior(False, False, "HighPriority")
        self.test_behavior_mid_priority = DummyBehavior(True, False, "MidPriority")
        self.test_behavior_low_priority = DummyBehavior(True, True, "LowPriority")

        self.environment_model = DummyEnvironmentModel()

        self.time = time.time()

    def test_default_verifier(self):
        test_priority_arbitrator = ag.PriorityArbitrator()

        test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_mid_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_low_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        self.assertTrue(
            test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )

        test_priority_arbitrator.gain_control(self.time, self.environment_model)

        self.assertEqual(
            "MidPriority",
            test_priority_arbitrator.get_command(self.time, self.environment_model),
        )

        result_0 = test_priority_arbitrator.options()[0].verification_result(self.time)
        result_1 = test_priority_arbitrator.options()[1].verification_result(self.time)
        result_2 = test_priority_arbitrator.options()[2].verification_result(self.time)
        self.assertFalse(result_0)
        self.assertFalse(result_1)
        self.assertTrue(result_2)
        # LowPriority could have been verified or not, so don't test it here

        result_2 = cast(Result, result_2)
        self.assertTrue(result_2.is_ok())

    def test_dummy_verifier_in_priority_arbitrator(self):
        test_priority_arbitrator = ag.PriorityArbitrator(
            "PriorityArbitrator", DummyVerifier("MidPriority")
        )

        test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_mid_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_low_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        self.assertTrue(
            test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )

        test_priority_arbitrator.gain_control(self.time, self.environment_model)

        self.assertEqual(
            "LowPriority",
            test_priority_arbitrator.get_command(self.time, self.environment_model),
        )

        result_0 = test_priority_arbitrator.options()[0].verification_result(self.time)
        result_1 = test_priority_arbitrator.options()[1].verification_result(self.time)
        result_2 = test_priority_arbitrator.options()[2].verification_result(self.time)
        result_3 = test_priority_arbitrator.options()[3].verification_result(self.time)
        self.assertFalse(result_0)
        self.assertFalse(result_1)
        self.assertTrue(result_2)
        self.assertTrue(result_3)

        result_2 = cast(Result, result_2)
        result_3 = cast(Result, result_3)
        self.assertFalse(result_2.is_ok())
        self.assertTrue(result_3.is_ok())

        # Print verification results
        print(
            f"Verification result for {test_priority_arbitrator.options()[2].behavior().name()}: "
            + f"{test_priority_arbitrator.options()[2].verification_result(self.time)}"
        )
        print(
            f"Verification result for {test_priority_arbitrator.options()[3].behavior().name()}: "
            + f"{test_priority_arbitrator.options()[3].verification_result(self.time)}"
        )

        # fmt:off
        ps = PrintStrings
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "PriorityArbitrator\n" +
            "    1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "    2. " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "    3. " + ps.strike_through_on
                      + ps.invocation_true + ps.commitment_false + "MidPriority"
                      + ps.strike_through_off + "\n" +
            " -> 4. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt:on
        actual_printout = test_priority_arbitrator.to_string(
            self.time, self.environment_model
        )
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        yaml_node = test_priority_arbitrator.to_yaml(self.time, self.environment_model)
        self.assertFalse("verificationResult" in yaml_node["options"][0])
        self.assertFalse("verificationResult" in yaml_node["options"][1])
        self.assertTrue("verificationResult" in yaml_node["options"][2])
        self.assertTrue("verificationResult" in yaml_node["options"][3])

        self.assertEqual("failed", yaml_node["options"][2]["verificationResult"])
        self.assertEqual("passed", yaml_node["options"][3]["verificationResult"])

        test_priority_arbitrator.lose_control(self.time, self.environment_model)

        self.test_behavior_low_priority.invocation_condition = False
        self.assertTrue(
            test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )

        test_priority_arbitrator.gain_control(self.time, self.environment_model)

        with self.assertRaises(ag.NoApplicableOptionPassedVerificationError):
            test_priority_arbitrator.get_command(self.time, self.environment_model)

    def test_dummy_verifier_in_priority_arbitrator_with_fallback(self):
        NO_FLAGS = ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        FALLBACK = ag.PriorityArbitrator.Option.Flags.FALLBACK

        test_priority_arbitrator = ag.PriorityArbitrator(
            "PriorityArbitrator", DummyVerifier("MidPriority")
        )

        test_priority_arbitrator.add_option(self.test_behavior_high_priority, NO_FLAGS)
        test_priority_arbitrator.add_option(self.test_behavior_high_priority, NO_FLAGS)
        test_priority_arbitrator.add_option(self.test_behavior_mid_priority, NO_FLAGS)
        test_priority_arbitrator.add_option(self.test_behavior_mid_priority, FALLBACK)
        test_priority_arbitrator.add_option(self.test_behavior_low_priority, NO_FLAGS)

        self.assertTrue(
            test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )

        test_priority_arbitrator.gain_control(self.time, self.environment_model)

        self.assertEqual(
            "MidPriority",
            test_priority_arbitrator.get_command(self.time, self.environment_model),
        )

        result_0 = test_priority_arbitrator.options()[0].verification_result(self.time)
        result_1 = test_priority_arbitrator.options()[1].verification_result(self.time)
        result_2 = test_priority_arbitrator.options()[2].verification_result(self.time)
        result_3 = test_priority_arbitrator.options()[3].verification_result(self.time)
        result_4 = test_priority_arbitrator.options()[4].verification_result(self.time)
        self.assertFalse(result_0)
        self.assertFalse(result_1)
        self.assertTrue(result_2)
        self.assertTrue(result_3)
        self.assertFalse(result_4)

        result_2 = cast(Result, result_2)
        result_3 = cast(Result, result_3)
        self.assertFalse(result_2.is_ok())
        self.assertFalse(result_3.is_ok())

        print(
            f"verificationResult for {test_priority_arbitrator.options()[2].behavior().name()}: "
            + f"{test_priority_arbitrator.options()[2].verification_result(self.time)}"
        )
        print(
            f"verificationResult for {test_priority_arbitrator.options()[3].behavior().name()}: "
            + f"{test_priority_arbitrator.options()[3].verification_result(self.time)}"
        )

        # fmt:off
        ps = PrintStrings
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "PriorityArbitrator\n" +
            "    1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "    2. " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "    3. " + ps.strike_through_on
                      + ps.invocation_true + ps.commitment_false + "MidPriority"
                      + ps.strike_through_off + "\n" +
            " -> 4. " + ps.strike_through_on
                      + ps.invocation_true + ps.commitment_false + "MidPriority"
                      + ps.strike_through_off + "\n" +
            "    5. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt:on
        actual_printout = test_priority_arbitrator.to_string(
            self.time, self.environment_model
        )
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        yaml_node = test_priority_arbitrator.to_yaml(self.time, self.environment_model)
        self.assertFalse("verificationResult" in yaml_node["options"][0])
        self.assertFalse("verificationResult" in yaml_node["options"][1])
        self.assertTrue("verificationResult" in yaml_node["options"][2])
        self.assertTrue("verificationResult" in yaml_node["options"][3])
        self.assertFalse("verificationResult" in yaml_node["options"][4])

        self.assertEqual("failed", yaml_node["options"][2]["verificationResult"])
        self.assertEqual("failed", yaml_node["options"][3]["verificationResult"])

    def test_dummy_verifier_in_cost_arbitrator(self):
        NO_FLAGS = ag.CostArbitrator.Option.Flags.NO_FLAGS

        test_cost_arbitrator = ag.CostArbitrator(
            "CostArbitrator", DummyVerifier("MidPriority")
        )

        cost_map = {
            DummyCommand("HighPriority"): 0,
            DummyCommand("MidPriority"): 0.5,
            DummyCommand("LowPriority"): 1,
        }
        cost_estimator = CostEstimatorFromCostMap(cost_map)

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

        self.assertTrue(
            test_cost_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )

        test_cost_arbitrator.gain_control(self.time, self.environment_model)

        self.assertEqual(
            "LowPriority",
            test_cost_arbitrator.get_command(self.time, self.environment_model),
        )
        result_0 = test_cost_arbitrator.options()[0].verification_result(self.time)
        result_1 = test_cost_arbitrator.options()[1].verification_result(self.time)
        result_2 = test_cost_arbitrator.options()[2].verification_result(self.time)
        result_3 = test_cost_arbitrator.options()[3].verification_result(self.time)

        self.assertFalse(result_0)
        self.assertFalse(result_1)
        self.assertTrue(result_2)
        self.assertTrue(result_3)

        # Let the type checker know that the results are not None
        result_2 = cast(Result, result_2)
        result_3 = cast(Result, result_3)

        self.assertFalse(result_2.is_ok())
        self.assertTrue(result_3.is_ok())

        # fmt:off
        ps = PrintStrings
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "CostArbitrator\n" +
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "    - (cost:  n.a.) " + ps.strike_through_on
                                   + ps.invocation_true + ps.commitment_false + "MidPriority"
                                   + ps.strike_through_off + "\n" +
            " -> - (cost: 1.000) " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt:on
        actual_printout = test_cost_arbitrator.to_string(
            self.time, self.environment_model
        )
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        test_cost_arbitrator.lose_control(self.time, self.environment_model)

        self.test_behavior_low_priority.invocation_condition = False
        self.assertTrue(
            test_cost_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )

        test_cost_arbitrator.gain_control(self.time, self.environment_model)

        with self.assertRaises(ag.NoApplicableOptionPassedVerificationError):
            test_cost_arbitrator.get_command(self.time, self.environment_model)
