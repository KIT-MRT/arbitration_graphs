import os
import time
import unittest

import arbitration_graphs as ag
from dummy_types import DummyBehavior, DummyEnvironmentModel, PrintStrings


class TestPriorityArbitrator(unittest.TestCase):
    def setUp(self):
        self.test_behavior_high_priority = DummyBehavior(False, False, "HighPriority")
        self.test_behavior_mid_priority = DummyBehavior(True, False, "MidPriority")
        self.test_behavior_low_priority = DummyBehavior(True, True, "LowPriority")

        self.test_priority_arbitrator = ag.PriorityArbitrator("PriorityArbitrator")

        self.environment_model = DummyEnvironmentModel()

        self.time = time.time()

    def test_basic_functionality(self):
        # Test if there are no options -> invocationCondition should be False
        self.assertFalse(
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
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
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
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
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
        )

        # Gain control and check command
        self.test_priority_arbitrator.gain_control(self.time, self.environment_model)
        self.assertEqual(
            "MidPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(0, self.test_behavior_mid_priority.lose_control_counter)

        # Mid priority behavior should lose control after the next get_command call
        self.assertEqual(
            "MidPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(1, self.test_behavior_mid_priority.lose_control_counter)

        # Now set mid-priority invocation to false and check again
        self.test_behavior_mid_priority.invocation_condition = False
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
        )
        self.assertEqual(
            "LowPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(0, self.test_behavior_low_priority.lose_control_counter)

        # Low priority behavior should not lose control if commitment is true
        self.assertEqual(
            "LowPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(0, self.test_behavior_low_priority.lose_control_counter)

        # Test with mid-priority invocation set to true again
        self.test_behavior_mid_priority.invocation_condition = True
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
        )
        self.assertEqual(
            "LowPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(
            "LowPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
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
        ps = PrintStrings()
        expected_printout = (
            ps.invocation_true + ps.commitment_false + "PriorityArbitrator\n"
            "    1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    2. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    3. " + ps.invocation_true + ps.commitment_false + "MidPriority\n"
            "    4. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt: on
        actual_printout = self.test_priority_arbitrator.to_string(
            self.time, self.environment_model
        )
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        self.test_priority_arbitrator.gain_control(self.time, self.environment_model)
        self.assertEqual(
            "MidPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )

        # fmt: off
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "PriorityArbitrator\n"
            "    1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            "    2. " + ps.invocation_false + ps.commitment_false + "HighPriority\n"
            " -> 3. " + ps.invocation_true + ps.commitment_false + "MidPriority\n"
            "    4. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt: on
        actual_printout = self.test_priority_arbitrator.to_string(
            self.time, self.environment_model
        )
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

        yaml_node = self.test_priority_arbitrator.to_yaml(
            self.time, self.environment_model
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

        self.test_priority_arbitrator.gain_control(self.time, self.environment_model)
        self.test_priority_arbitrator.get_command(self.time, self.environment_model)

        yaml_node = self.test_priority_arbitrator.to_yaml(
            self.time, self.environment_model
        )

        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])

        self.assertTrue("activeBehavior" in yaml_node)
        self.assertEqual(2, yaml_node["activeBehavior"])

    def test_basic_functionality_with_interruptable_options(self):
        # if there are no options yet -> the invocationCondition should be false
        self.assertFalse(
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
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
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
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
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertFalse(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
        )

        self.test_priority_arbitrator.gain_control(self.time, self.environment_model)
        self.assertEqual(
            "MidPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(
            "MidPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )

        self.test_behavior_mid_priority.invocation_condition = False
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
        )
        self.assertEqual(
            "LowPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(
            "LowPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )

        self.test_behavior_mid_priority.invocation_condition = True
        self.assertTrue(
            self.test_priority_arbitrator.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertTrue(
            self.test_priority_arbitrator.check_commitment_condition(
                self.time, self.environment_model
            )
        )
        self.assertEqual(
            "MidPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
        self.assertEqual(
            "MidPriority",
            self.test_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )
