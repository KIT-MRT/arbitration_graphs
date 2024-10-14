import os
import time
import unittest

import yaml

import arbitration_graphs_py as ag
from dummy_types import DummyBehaviorPy, PrintStrings

import unittest
import time


class ConjunctiveCoordinatorTest(unittest.TestCase):
    def setUp(self):
        self.test_behavior_a = DummyBehaviorPy(False, False, "A")
        self.test_behavior_b1 = DummyBehaviorPy(True, False, "B")
        self.test_behavior_c = DummyBehaviorPy(True, True, "C")
        self.test_behavior_b2 = DummyBehaviorPy(True, False, "B")

        self.test_conjunctive_coordinator = ag.ConjunctiveCoordinator()

        self.time = time.time()

    def test_basic_functionality(self):
        # if there are no options yet -> the invocationCondition should be false
        self.assertFalse(
            self.test_conjunctive_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_conjunctive_coordinator.check_commitment_condition(self.time)
        )

        # otherwise the invocationCondition is true if all of the option have true invocationCondition
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_a, ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS
        )
        self.assertFalse(
            self.test_conjunctive_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_conjunctive_coordinator.check_commitment_condition(self.time)
        )

        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_b1, ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS
        )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_c, ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS
        )

        self.assertFalse(
            self.test_conjunctive_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_conjunctive_coordinator.check_commitment_condition(self.time)
        )

        # make all options invocationCondition true now -> the coordinators invocationCondition should also be true
        self.test_behavior_a.invocation_condition = True
        self.assertTrue(
            self.test_conjunctive_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_conjunctive_coordinator.check_commitment_condition(self.time)
        )

        # coordinators cannot hold the same behavior instantiation multiple times
        with self.assertRaises(ag.MultipleReferencesToSameInstanceError):
            self.test_conjunctive_coordinator.add_option(
                self.test_behavior_b1, ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS
            )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_b2, ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS
        )

        # if any option has true commitmentCondition and the coordinator is active, i.e. gained control
        # -> the coordinators commitmentCondition should also be true
        self.test_conjunctive_coordinator.gain_control(self.time)
        self.assertTrue(
            self.test_conjunctive_coordinator.check_invocation_condition(self.time)
        )
        self.assertTrue(
            self.test_conjunctive_coordinator.check_commitment_condition(self.time)
        )

        # the ConjunctiveCoordinator joins all sub-commands using the "& operator"
        self.assertEqual(
            "ABCB", self.test_conjunctive_coordinator.get_command(self.time)
        )
        self.assertEqual(0, self.test_behavior_a.lose_control_counter)
        self.assertEqual(0, self.test_behavior_b1.lose_control_counter)
        self.assertEqual(0, self.test_behavior_c.lose_control_counter)

        # loseControl(time) should never be called within getCommand of the ConjunctiveCoordinator as the sub-behaviors
        # gain control with the ConjunctiveCoordinator altogether ...
        self.assertEqual(
            "ABCB", self.test_conjunctive_coordinator.get_command(self.time)
        )
        self.assertEqual(0, self.test_behavior_a.lose_control_counter)
        self.assertEqual(0, self.test_behavior_b1.lose_control_counter)
        self.assertEqual(0, self.test_behavior_c.lose_control_counter)

        # ... even if all of the sub-behaviors commitmentCondition are false
        self.test_behavior_c.commitment_condition = False
        self.assertEqual(
            "ABCB", self.test_conjunctive_coordinator.get_command(self.time)
        )
        self.assertEqual(0, self.test_behavior_a.lose_control_counter)
        self.assertEqual(0, self.test_behavior_b1.lose_control_counter)
        self.assertEqual(0, self.test_behavior_c.lose_control_counter)

        self.assertTrue(
            self.test_conjunctive_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_conjunctive_coordinator.check_commitment_condition(self.time)
        )

        # ConjunctiveCoordinators loseControl(time) should also call loseControl(time) for all its sub-behaviors
        self.test_conjunctive_coordinator.lose_control(self.time)
        self.assertEqual(1, self.test_behavior_a.lose_control_counter)
        self.assertEqual(1, self.test_behavior_b1.lose_control_counter)
        self.assertEqual(1, self.test_behavior_c.lose_control_counter)

    def test_printout(self):
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_a,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_b1,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_c,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_b2,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )

        # fmt: off
        ps = PrintStrings()
        expected_printout = (
            ps.invocation_false + ps.commitment_false + "ConjunctiveCoordinator\n"
            "    - " + ps.invocation_false + ps.commitment_false + "A\n"
            "    - " + ps.invocation_true + ps.commitment_false + "B\n"
            "    - " + ps.invocation_true + ps.commitment_true + "C\n"
            "    - " + ps.invocation_true + ps.commitment_false + "B"
        )
        # fmt: on
        actual_printout = self.test_conjunctive_coordinator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        self.test_behavior_a.invocation_condition = True
        self.test_conjunctive_coordinator.gain_control(self.time)
        self.assertEqual(
            "ABCB", self.test_conjunctive_coordinator.get_command(self.time)
        )

        # fmt: off
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "ConjunctiveCoordinator\n"
            " -> - " + ps.invocation_true + ps.commitment_false + "A\n"
            " -> - " + ps.invocation_true + ps.commitment_false + "B\n"
            " -> - " + ps.invocation_true + ps.commitment_true + "C\n"
            " -> - " + ps.invocation_true + ps.commitment_false + "B"
        )
        # fmt: on
        actual_printout = self.test_conjunctive_coordinator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_a,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_b1,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_c,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_conjunctive_coordinator.add_option(
            self.test_behavior_b2,
            ag.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )

        yaml_node = yaml.safe_load(
            self.test_conjunctive_coordinator.to_yaml_as_str(self.time)
        )

        self.assertEqual("ConjunctiveCoordinator", yaml_node["type"])
        self.assertEqual("ConjunctiveCoordinator", yaml_node["name"])
        self.assertFalse(yaml_node["invocationCondition"])
        self.assertFalse(yaml_node["commitmentCondition"])

        self.assertEqual(4, len(yaml_node["options"]))
        self.assertEqual("Option", yaml_node["options"][0]["type"])
        self.assertEqual("Option", yaml_node["options"][1]["type"])
        self.assertEqual("Option", yaml_node["options"][2]["type"])
        self.assertEqual("Option", yaml_node["options"][3]["type"])
        self.assertEqual("A", yaml_node["options"][0]["behavior"]["name"])
        self.assertEqual("B", yaml_node["options"][1]["behavior"]["name"])
        self.assertEqual("C", yaml_node["options"][2]["behavior"]["name"])
        self.assertEqual("B", yaml_node["options"][3]["behavior"]["name"])
        self.assertFalse(yaml_node["options"][0]["behavior"]["invocationCondition"])
        self.assertTrue(yaml_node["options"][1]["behavior"]["invocationCondition"])
        self.assertTrue(yaml_node["options"][2]["behavior"]["invocationCondition"])
        self.assertTrue(yaml_node["options"][3]["behavior"]["invocationCondition"])
        self.assertFalse(yaml_node["options"][0]["behavior"]["commitmentCondition"])
        self.assertFalse(yaml_node["options"][1]["behavior"]["commitmentCondition"])
        self.assertTrue(yaml_node["options"][2]["behavior"]["commitmentCondition"])
        self.assertFalse(yaml_node["options"][3]["behavior"]["commitmentCondition"])

        self.test_behavior_a.invocation_condition = True
        self.test_conjunctive_coordinator.gain_control(self.time)
        self.test_conjunctive_coordinator.get_command(self.time)

        yaml_node = yaml.safe_load(
            self.test_conjunctive_coordinator.to_yaml_as_str(self.time)
        )

        # Assertions after gaining control
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])

        self.assertEqual(4, len(yaml_node["options"]))
        self.assertTrue(yaml_node["options"][0]["behavior"]["invocationCondition"])
        self.assertTrue(yaml_node["options"][1]["behavior"]["invocationCondition"])
        self.assertTrue(yaml_node["options"][2]["behavior"]["invocationCondition"])
        self.assertTrue(yaml_node["options"][3]["behavior"]["invocationCondition"])
        self.assertFalse(yaml_node["options"][0]["behavior"]["commitmentCondition"])
        self.assertFalse(yaml_node["options"][1]["behavior"]["commitmentCondition"])
        self.assertTrue(yaml_node["options"][2]["behavior"]["commitmentCondition"])
        self.assertFalse(yaml_node["options"][3]["behavior"]["commitmentCondition"])


if __name__ == "__main__":
    header = "Running " + os.path.basename(__file__)

    print("=" * len(header))
    print(header)
    print("=" * len(header) + "\n")
    unittest.main(exit=False)
    print("=" * len(header) + "\n")
