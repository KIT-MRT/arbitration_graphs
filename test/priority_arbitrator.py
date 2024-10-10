import sys
import os

build_directory = "/home/blinky/arbitration_graphs/build/"
sys.path.append(build_directory)

import time
import unittest

import yaml

import arbitration_graphs_py as ag
from dummy_types import DummyBehaviorPy, DummyCommandPy


class TestPriorityArbitrator(unittest.TestCase):
    def setUp(self):
        self.test_behavior_high_priority = DummyBehaviorPy(False, False, "HighPriority")
        self.test_behavior_mid_priority = DummyBehaviorPy(True, False, "MidPriority")
        self.test_behavior_low_priority = DummyBehaviorPy(True, True, "LowPriority")

        self.test_priority_arbitrator = ag.PriorityArbitrator("PriorityArbitrator")

        self.time = time.time()

    def test_basic_functionality(self):
        # Test if there are no options -> invocationCondition should be False
        self.assertFalse(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )

        # Adding options but no invocation
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )

        # Adding mid and low priority options
        self.test_priority_arbitrator.add_option(
            self.test_behavior_mid_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_low_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )
        # Invocation should be true but not commitment
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )

        # Gain control and check command
        self.test_priority_arbitrator.gain_control(self.time)
        self.assertEqual(
            "MidPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(0, self.test_behavior_mid_priority.lose_control_counter)

        # Mid priority behavior should lose control after the next get_command call
        self.assertEqual(
            "MidPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(1, self.test_behavior_mid_priority.lose_control_counter)

        # Now set mid-priority invocation to false and check again
        self.test_behavior_mid_priority.invocation_condition = False
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )
        self.assertEqual(
            "LowPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(0, self.test_behavior_low_priority.lose_control_counter)

        # Low priority behavior should not lose control if commitment is true
        self.assertEqual(
            "LowPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(0, self.test_behavior_low_priority.lose_control_counter)

        # Test with mid-priority invocation set to true again
        self.test_behavior_mid_priority.invocation_condition = True
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )
        self.assertEqual(
            "LowPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(
            "LowPriority", self.test_priority_arbitrator.get_command(self.time)
        )

    def test_printout(self):
        # Add options to the arbitrator
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_mid_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_low_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )

        # fmt: off
        expected_printout = (
            ag.testing_types.invocation_true_string + ag.testing_types.commitment_false_string + "PriorityArbitrator\n"
            "    1. " + ag.testing_types.invocation_false_string + ag.testing_types.commitment_false_string + "HighPriority\n"
            "    2. " + ag.testing_types.invocation_false_string + ag.testing_types.commitment_false_string + "HighPriority\n"
            "    3. " + ag.testing_types.invocation_true_string + ag.testing_types.commitment_false_string + "MidPriority\n"
            "    4. " + ag.testing_types.invocation_true_string + ag.testing_types.commitment_true_string + "LowPriority"
        )
        # fmt: on
        actual_printout = self.test_priority_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        self.test_priority_arbitrator.gain_control(self.time)
        self.assertEqual(
            "MidPriority", self.test_priority_arbitrator.get_command(self.time)
        )

        # fmt: off
        expected_printout = (
            ag.testing_types.invocation_true_string + ag.testing_types.commitment_true_string + "PriorityArbitrator\n"
            "    1. " + ag.testing_types.invocation_false_string + ag.testing_types.commitment_false_string + "HighPriority\n"
            "    2. " + ag.testing_types.invocation_false_string + ag.testing_types.commitment_false_string + "HighPriority\n"
            " -> 3. " + ag.testing_types.invocation_true_string + ag.testing_types.commitment_false_string + "MidPriority\n"
            "    4. " + ag.testing_types.invocation_true_string + ag.testing_types.commitment_true_string + "LowPriority"
        )
        # fmt: on
        actual_printout = self.test_priority_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        # Add options to the arbitrator
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_mid_priority,
            ag.PriorityArbitrator.Option.Flags.INTERRUPTABLE,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_low_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )

        yaml_node = yaml.safe_load(
            self.test_priority_arbitrator.to_yaml_as_str(self.time)
        )

        self.assertEqual("PriorityArbitrator", yaml_node["type"])
        self.assertEqual("PriorityArbitrator", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertFalse(yaml_node["commitmentCondition"])

        self.assertEqual(4, len(yaml_node["options"]))
        self.assertEqual("Option", yaml_node["options"][0]["type"])
        self.assertEqual("Option", yaml_node["options"][1]["type"])
        self.assertEqual("Option", yaml_node["options"][2]["type"])
        self.assertEqual("Option", yaml_node["options"][3]["type"])
        self.assertEqual("HighPriority", yaml_node["options"][0]["behavior"]["name"])
        self.assertEqual("HighPriority", yaml_node["options"][1]["behavior"]["name"])
        self.assertEqual("MidPriority", yaml_node["options"][2]["behavior"]["name"])
        self.assertEqual("LowPriority", yaml_node["options"][3]["behavior"]["name"])
        self.assertFalse("flags" in yaml_node["options"][0])
        self.assertFalse("flags" in yaml_node["options"][1])
        self.assertTrue("flags" in yaml_node["options"][2])
        self.assertFalse("flags" in yaml_node["options"][3])

        self.assertFalse("activeBehavior" in yaml_node)

        self.test_priority_arbitrator.gain_control(self.time)
        self.test_priority_arbitrator.get_command(self.time)

        yaml_node = yaml.safe_load(
            self.test_priority_arbitrator.to_yaml_as_str(self.time)
        )

        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])

        self.assertTrue("activeBehavior" in yaml_node)
        self.assertEqual(2, yaml_node["activeBehavior"])

    def test_basic_functionality_with_interruptable_options(self):
        # if there are no options yet -> the invocationCondition should be false
        self.assertFalse(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )

        # otherwise the invocationCondition is true if any of the option has true invocationCondition
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.INTERRUPTABLE,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.INTERRUPTABLE,
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )

        self.test_priority_arbitrator.add_option(
            self.test_behavior_mid_priority,
            ag.PriorityArbitrator.Option.Flags.INTERRUPTABLE,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_low_priority,
            ag.PriorityArbitrator.Option.Flags.INTERRUPTABLE,
        )

        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )

        self.test_priority_arbitrator.gain_control(self.time)
        self.assertEqual(
            "MidPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(
            "MidPriority", self.test_priority_arbitrator.get_command(self.time)
        )

        self.test_behavior_mid_priority.invocation_condition = False
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )
        self.assertEqual(
            "LowPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(
            "LowPriority", self.test_priority_arbitrator.get_command(self.time)
        )

        self.test_behavior_mid_priority.invocation_condition = True
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(self.time)
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(self.time)
        )
        self.assertEqual(
            "MidPriority", self.test_priority_arbitrator.get_command(self.time)
        )
        self.assertEqual(
            "MidPriority", self.test_priority_arbitrator.get_command(self.time)
        )

    # def test_sub_command_type_differs_from_command_type(self):
    #     # Different sub-command type case
    #     test_priority_arbitrator = PriorityArbitrator("test_arbitrator_with_subcommand")
    #     test_behavior_high_priority = DummyBehavior(False, False, "___HighPriority___")
    #     test_behavior_mid_priority = DummyBehavior(True, False, "__MidPriority__")
    #     test_behavior_low_priority = DummyBehavior(True, True, "_LowPriority_")
    #
    #     test_priority_arbitrator.add_option(
    #         test_behavior_high_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
    #     )
    #     test_priority_arbitrator.add_option(
    #         test_behavior_high_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
    #     )
    #     test_priority_arbitrator.add_option(
    #         test_behavior_mid_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
    #     )
    #     test_priority_arbitrator.add_option(
    #         test_behavior_low_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
    #     )
    #
    #     test_priority_arbitrator.gain_control(self.time)
    #
    #     expected = "__MidPriority__"
    #     self.assertEqual(len(expected), test_priority_arbitrator.get_command(self.time))


if __name__ == "__main__":
    unittest.main()
