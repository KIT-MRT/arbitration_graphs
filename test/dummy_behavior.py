import time
import unittest

import yaml

import arbitration_graphs_py as ag
from dummy_types import DummyBehaviorPy, DummyCommandPy, PrintStrings


class DummyBehaviorTest(unittest.TestCase):

    def setUp(self):
        self.test_behavior_true = ag.testing_types.DummyBehavior(True, True)

        self.expected_command = ag.testing_types.DummyCommand("dummy_behavior")

        self.time = time.time()

    def test_basic_interface(self):
        self.assertEqual(
            self.expected_command, self.test_behavior_true.get_command(self.time)
        )
        self.assertTrue(self.test_behavior_true.check_commitment_condition(self.time))
        self.assertTrue(self.test_behavior_true.check_invocation_condition(self.time))

        try:
            self.test_behavior_true.gain_control(self.time)
        except Exception as e:
            self.fail(f"gain_control threw an exception: {e}")
        try:
            self.test_behavior_true.lose_control(self.time)
        except Exception as e:
            self.fail(f"lose_control threw an exception: {e}")

    def test_printout(self):
        expected_printout = (
            ag.testing_types.invocation_true_string
            + ag.testing_types.commitment_true_string
            + "dummy_behavior"
        )
        actual_printout = self.test_behavior_true.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        yaml_node = yaml.safe_load(self.test_behavior_true.to_yaml_as_str(self.time))

        self.assertEqual("Behavior", yaml_node["type"])
        self.assertEqual("dummy_behavior", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])


class DummyBehaviorPyTest(unittest.TestCase):

    def setUp(self):
        self.test_behavior_true = DummyBehaviorPy(True, True)

        self.expected_command = DummyCommandPy("dummy_behavior")

        self.time = time.time()

    def test_basic_interface(self):
        self.assertEqual(
            self.expected_command, self.test_behavior_true.get_command(self.time)
        )
        self.assertTrue(self.test_behavior_true.check_commitment_condition(self.time))
        self.assertTrue(self.test_behavior_true.check_invocation_condition(self.time))

        try:
            self.test_behavior_true.gain_control(self.time)
        except Exception as e:
            self.fail(f"gain_control threw an exception: {e}")
        try:
            self.test_behavior_true.lose_control(self.time)
        except Exception as e:
            self.fail(f"lose_control threw an exception: {e}")

    def test_printout(self):
        expected_printout = (
            PrintStrings.invocation_true_string
            + PrintStrings.commitment_true_string
            + "dummy_behavior"
        )
        actual_printout = self.test_behavior_true.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        yaml_node = yaml.safe_load(self.test_behavior_true.to_yaml_as_str(self.time))

        self.assertEqual("Behavior", yaml_node["type"])
        self.assertEqual("dummy_behavior", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])


if __name__ == "__main__":
    unittest.main()