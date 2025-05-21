import os
import time
import unittest

import arbitration_graphs as ag
from dummy_types import DummyBehavior, DummyCommand, PrintStrings


class DummyBehaviorTest(unittest.TestCase):

    def setUp(self):
        self.test_behavior_true = DummyBehavior(True, True)

        self.expected_command = DummyCommand("dummy_behavior")

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
        ps = PrintStrings()
        expected_printout = ps.invocation_true + ps.commitment_true + "dummy_behavior"
        actual_printout = self.test_behavior_true.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        yaml_node = self.test_behavior_true.to_yaml(self.time)

        self.assertEqual("Behavior", yaml_node["type"])
        self.assertEqual("dummy_behavior", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])
