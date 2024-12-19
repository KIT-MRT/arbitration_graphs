import os
import time
import unittest
import yaml

import arbitration_graphs_py as ag
from dummy_types import DummyBehaviorPy, PrintStrings


class JointCoordinatorTest(unittest.TestCase):
    def setUp(self):
        self.test_behavior_a = DummyBehaviorPy(False, False, "A")
        self.test_behavior_b1 = DummyBehaviorPy(True, False, "B")
        self.test_behavior_c = DummyBehaviorPy(True, True, "C")
        self.test_behavior_b2 = DummyBehaviorPy(True, False, "B")

        self.test_joint_coordinator = ag.JointCoordinator()

        self.time = time.time()

    def test_basic_functionality(self):
        # if there are no options yet -> the invocationCondition should be false
        self.assertFalse(
            self.test_joint_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_joint_coordinator.check_commitment_condition(self.time)
        )

        # otherwise the invocationCondition is true if any of the option has true invocationCondition
        self.test_joint_coordinator.add_option(
            self.test_behavior_a, ag.JointCoordinator.Option.Flags.NO_FLAGS
        )
        self.assertFalse(
            self.test_joint_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_joint_coordinator.check_commitment_condition(self.time)
        )

        self.test_joint_coordinator.add_option(
            self.test_behavior_b1, ag.JointCoordinator.Option.Flags.NO_FLAGS
        )
        self.test_joint_coordinator.add_option(
            self.test_behavior_c, ag.JointCoordinator.Option.Flags.NO_FLAGS
        )

        self.assertTrue(
            self.test_joint_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_joint_coordinator.check_commitment_condition(self.time)
        )

        self.test_joint_coordinator.gain_control(self.time)
        self.assertEqual("BC", self.test_joint_coordinator.get_command(self.time))

        actual_printout = self.test_joint_coordinator.to_str(self.time)
        print(actual_printout)

        # if any active option has true commitmentCondition and the coordinator is active, i.e. gained control
        # -> the coordinators commitmentCondition should also be true
        self.assertTrue(
            self.test_joint_coordinator.check_invocation_condition(self.time)
        )
        self.assertTrue(
            self.test_joint_coordinator.check_commitment_condition(self.time)
        )

        self.assertEqual("BC", self.test_joint_coordinator.get_command(self.time))

        # lets make it a little more complicated...
        self.test_behavior_a.commitment_condition = True
        self.test_behavior_b1.invocation_condition = False
        self.test_behavior_c.invocation_condition = False
        self.assertFalse(
            self.test_joint_coordinator.check_invocation_condition(self.time)
        )
        self.assertTrue(
            self.test_joint_coordinator.check_commitment_condition(self.time)
        )

        # A should not be selected:
        # - it has false invocation condition and
        # - was inactive before (thus, its true commitment condition should have no effect)
        # B should not be selected:
        # - it has false invocation condition and
        # - was active before, but has false commitment condition
        # C should be selected:
        # - it has false invocation condition, but
        # - it was active before and has true commitment condition
        self.assertEqual("C", self.test_joint_coordinator.get_command(self.time))

        # make all options invocationCondition false now -> the coordinators invocationCondition should also be false
        self.test_behavior_b1.invocation_condition = False
        self.test_behavior_c.invocation_condition = False
        self.test_behavior_c.commitment_condition = False
        self.assertFalse(
            self.test_joint_coordinator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_joint_coordinator.check_commitment_condition(self.time)
        )

        # coordinators cannot hold the same behavior instantiation multiple times
        with self.assertRaises(ag.MultipleReferencesToSameInstanceError):
            self.test_joint_coordinator.add_option(
                self.test_behavior_b1, ag.JointCoordinator.Option.Flags.NO_FLAGS
            )
        self.test_joint_coordinator.add_option(
            self.test_behavior_b2, ag.JointCoordinator.Option.Flags.NO_FLAGS
        )

        # JointCoordinator loseControl(time) should also call loseControl(time) for all its sub-behaviors
        self.test_joint_coordinator.lose_control(self.time)
        self.assertEqual(0, self.test_behavior_a.lose_control_counter)
        self.assertEqual(1, self.test_behavior_b1.lose_control_counter)
        self.assertEqual(1, self.test_behavior_c.lose_control_counter)

    def test_printout(self):
        self.test_joint_coordinator.add_option(
            self.test_behavior_a,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_joint_coordinator.add_option(
            self.test_behavior_b1,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_joint_coordinator.add_option(
            self.test_behavior_c,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_joint_coordinator.add_option(
            self.test_behavior_b2,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )

        # fmt: off
        ps = PrintStrings()
        expected_printout = (
            ps.invocation_true + ps.commitment_false + "JointCoordinator\n"
            "    - " + ps.invocation_false + ps.commitment_false + "A\n"
            "    - " + ps.invocation_true + ps.commitment_false + "B\n"
            "    - " + ps.invocation_true + ps.commitment_true + "C\n"
            "    - " + ps.invocation_true + ps.commitment_false + "B"
        )
        # fmt: on
        actual_printout = self.test_joint_coordinator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        self.test_joint_coordinator.gain_control(self.time)
        self.assertEqual("BCB", self.test_joint_coordinator.get_command(self.time))

        # fmt: off
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "JointCoordinator\n"
            "    - " + ps.invocation_false + ps.commitment_false + "A\n"
            " -> - " + ps.invocation_true + ps.commitment_false + "B\n"
            " -> - " + ps.invocation_true + ps.commitment_true + "C\n"
            " -> - " + ps.invocation_true + ps.commitment_false + "B"
        )
        # fmt: on
        actual_printout = self.test_joint_coordinator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        self.test_joint_coordinator.add_option(
            self.test_behavior_a,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_joint_coordinator.add_option(
            self.test_behavior_b1,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_joint_coordinator.add_option(
            self.test_behavior_c,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )
        self.test_joint_coordinator.add_option(
            self.test_behavior_b2,
            ag.JointCoordinator.Option.Flags.NO_FLAGS,
        )

        yaml_node = yaml.safe_load(
            self.test_joint_coordinator.to_yaml_as_str(self.time)
        )

        self.assertEqual("JointCoordinator", yaml_node["type"])
        self.assertEqual("JointCoordinator", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
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
        self.assertFalse("flags" in yaml_node["options"][0])
        self.assertFalse("flags" in yaml_node["options"][1])
        self.assertFalse("flags" in yaml_node["options"][2])
        self.assertFalse("flags" in yaml_node["options"][3])

        self.test_behavior_a.invocation_condition = True
        self.test_joint_coordinator.gain_control(self.time)
        self.test_joint_coordinator.get_command(self.time)

        yaml_node = yaml.safe_load(
            self.test_joint_coordinator.to_yaml_as_str(self.time)
        )

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
