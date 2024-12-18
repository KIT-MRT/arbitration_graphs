import os
import time
import unittest
import yaml

from collections import defaultdict

import arbitration_graphs_py as ag
from dummy_types import DummyBehaviorPy, DummyCommandPy, PrintStrings


class TestRandomArbitrator(unittest.TestCase):
    def setUp(self):
        self.test_behavior_unavailable = DummyBehaviorPy(False, False, "Unavailable")
        self.test_behavior_high_weight = DummyBehaviorPy(True, False, "HighWeight")
        self.test_behavior_mid_weight = DummyBehaviorPy(True, False, "MidWeight")
        self.test_behavior_low_weight = DummyBehaviorPy(True, False, "LowWeight")

        self.test_random_arbitrator = ag.RandomArbitrator()

        self.time = time.time()

    def test_basic_functionality(self):
        NO_FLAGS = ag.RandomArbitrator.Option.Flags.NO_FLAGS

        # if there are no options yet -> the invocationCondition should be false
        self.assertFalse(
            self.test_random_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_random_arbitrator.check_commitment_condition(self.time)
        )

        # otherwise the invocationCondition is true if any of the option has true invocationCondition
        self.test_random_arbitrator.add_option(self.test_behavior_unavailable, NO_FLAGS)
        self.assertFalse(
            self.test_random_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_random_arbitrator.check_commitment_condition(self.time)
        )

        self.test_random_arbitrator.add_option(
            self.test_behavior_high_weight, NO_FLAGS, 2.5
        )
        self.test_random_arbitrator.add_option(self.test_behavior_mid_weight, NO_FLAGS)
        self.test_random_arbitrator.add_option(
            self.test_behavior_low_weight, NO_FLAGS, 0.5
        )
        weight_sum_of_available_options = 2.5 + 1.0 + 0.5

        self.assertTrue(
            self.test_random_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_random_arbitrator.check_commitment_condition(self.time)
        )

        self.test_random_arbitrator.gain_control(self.time)

        sample_size = 1000
        command_counter = defaultdict(int)

        for _ in range(sample_size):
            command = self.test_random_arbitrator.get_command(self.time)
            command_counter[command] += 1

        self.assertEqual(0, command_counter["Unavailable"])
        self.assertAlmostEqual(
            2.5 / weight_sum_of_available_options,
            command_counter["HighWeight"] / sample_size,
            delta=0.1,
        )
        self.assertAlmostEqual(
            1.0 / weight_sum_of_available_options,
            command_counter["MidWeight"] / sample_size,
            delta=0.1,
        )
        self.assertAlmostEqual(
            0.5 / weight_sum_of_available_options,
            command_counter["LowWeight"] / sample_size,
            delta=0.1,
        )

    def test_printout(self):
        NO_FLAGS = ag.RandomArbitrator.Option.Flags.NO_FLAGS

        # Force midWeight behavior by setting other behaviors' weights to 0
        self.test_random_arbitrator.add_option(self.test_behavior_unavailable, NO_FLAGS)
        self.test_random_arbitrator.add_option(
            self.test_behavior_high_weight, NO_FLAGS, 0
        )
        self.test_random_arbitrator.add_option(
            self.test_behavior_high_weight, NO_FLAGS, 0
        )
        self.test_random_arbitrator.add_option(
            self.test_behavior_mid_weight, NO_FLAGS, 2.5
        )
        self.test_random_arbitrator.add_option(
            self.test_behavior_low_weight, NO_FLAGS, 0
        )

        # fmt:off
        ps = PrintStrings()
        expected_printout = (
            ps.invocation_true + ps.commitment_false + "RandomArbitrator\n"
            "    - (weight: 1.000) " + ps.invocation_false + ps.commitment_false + "Unavailable\n"
            "    - (weight: 0.000) " + ps.invocation_true + ps.commitment_false + "HighWeight\n"
            "    - (weight: 0.000) " + ps.invocation_true + ps.commitment_false + "HighWeight\n"
            "    - (weight: 2.500) " + ps.invocation_true + ps.commitment_false + "MidWeight\n"
            "    - (weight: 0.000) " + ps.invocation_true + ps.commitment_false + "LowWeight"
        )
        # fmt:on
        actual_printout = self.test_random_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        self.test_random_arbitrator.gain_control(self.time)
        self.assertEqual(
            "MidWeight", self.test_random_arbitrator.get_command(self.time)
        )

        # fmt:off
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "RandomArbitrator\n"
            "    - (weight: 1.000) " + ps.invocation_false + ps.commitment_false + "Unavailable\n"
            "    - (weight: 0.000) " + ps.invocation_true + ps.commitment_false + "HighWeight\n"
            "    - (weight: 0.000) " + ps.invocation_true + ps.commitment_false + "HighWeight\n"
            " -> - (weight: 2.500) " + ps.invocation_true + ps.commitment_false + "MidWeight\n"
            "    - (weight: 0.000) " + ps.invocation_true + ps.commitment_false + "LowWeight"
        )
        # fmt:on
        actual_printout_after_gain = self.test_random_arbitrator.to_str(self.time)
        print(actual_printout_after_gain)

        self.assertEqual(expected_printout, actual_printout_after_gain)

    def test_to_yaml(self):
        INTERRUPTABLE = ag.RandomArbitrator.Option.Flags.INTERRUPTABLE
        NO_FLAGS = ag.RandomArbitrator.Option.Flags.NO_FLAGS

        self.test_random_arbitrator.add_option(self.test_behavior_unavailable, NO_FLAGS)
        self.test_random_arbitrator.add_option(
            self.test_behavior_high_weight, NO_FLAGS, 0
        )
        self.test_random_arbitrator.add_option(
            self.test_behavior_high_weight, NO_FLAGS, 0
        )
        self.test_random_arbitrator.add_option(
            self.test_behavior_mid_weight, INTERRUPTABLE
        )
        self.test_random_arbitrator.add_option(
            self.test_behavior_low_weight, NO_FLAGS, 0
        )

        yaml_node = yaml.safe_load(
            self.test_random_arbitrator.to_yaml_as_str(self.time)
        )

        self.assertEqual("RandomArbitrator", yaml_node["type"])
        self.assertEqual("RandomArbitrator", yaml_node["name"])
        self.assertEqual(True, yaml_node["invocationCondition"])
        self.assertEqual(False, yaml_node["commitmentCondition"])

        self.assertEqual(5, len(yaml_node["options"]))
        self.assertEqual("Option", yaml_node["options"][0]["type"])
        self.assertEqual("Option", yaml_node["options"][1]["type"])
        self.assertEqual("Option", yaml_node["options"][2]["type"])
        self.assertEqual("Option", yaml_node["options"][3]["type"])
        self.assertEqual("Option", yaml_node["options"][4]["type"])
        self.assertEqual("Unavailable", yaml_node["options"][0]["behavior"]["name"])
        self.assertEqual("HighWeight", yaml_node["options"][1]["behavior"]["name"])
        self.assertEqual("HighWeight", yaml_node["options"][2]["behavior"]["name"])
        self.assertEqual("MidWeight", yaml_node["options"][3]["behavior"]["name"])
        self.assertEqual("LowWeight", yaml_node["options"][4]["behavior"]["name"])
        self.assertFalse("flags" in yaml_node["options"][0])
        self.assertFalse("flags" in yaml_node["options"][1])
        self.assertFalse("flags" in yaml_node["options"][2])
        self.assertTrue("flags" in yaml_node["options"][3])
        self.assertFalse("flags" in yaml_node["options"][4])

        self.assertTrue("activeBehavior" not in yaml_node)

        self.test_random_arbitrator.gain_control(self.time)
        self.test_random_arbitrator.get_command(self.time)

        yaml_node = yaml.safe_load(
            self.test_random_arbitrator.to_yaml_as_str(self.time)
        )

        self.assertEqual(True, yaml_node["invocationCondition"])
        self.assertEqual(True, yaml_node["commitmentCondition"])

        self.assertTrue("activeBehavior" in yaml_node)
        self.assertEqual(3, yaml_node["activeBehavior"])


if __name__ == "__main__":
    header = "Running " + os.path.basename(__file__)

    print("=" * len(header))
    print(header)
    print("=" * len(header) + "\n")
    unittest.main(exit=False)
    print("=" * len(header) + "\n")
