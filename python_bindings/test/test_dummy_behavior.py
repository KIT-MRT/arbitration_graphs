# pyright: reportUninitializedInstanceVariable=false

import time
import unittest
from typing import final

from typing_extensions import override

from .dummy_types import (
    DummyBehavior,
    DummyCommand,
    DummyEnvironmentModel,
    PrintStrings,
)


@final
class DummyBehaviorTest(unittest.TestCase):
    @override
    def setUp(self):
        self.test_behavior_true = DummyBehavior(True, True)
        self.environment_model = DummyEnvironmentModel()

        self.expected_command = DummyCommand("dummy_behavior")

        self.time = time.time()

    def test_basic_interface(self):
        self.assertEqual(0, self.environment_model.access_counter)
        self.assertEqual(
            self.expected_command,
            self.test_behavior_true.get_command(self.time, self.environment_model),
        )
        self.assertEqual(1, self.environment_model.access_counter)
        self.assertTrue(
            self.test_behavior_true.check_commitment_condition(
                self.time, self.environment_model
            )
        )
        self.assertEqual(2, self.environment_model.access_counter)
        self.assertTrue(
            self.test_behavior_true.check_invocation_condition(
                self.time, self.environment_model
            )
        )
        self.assertEqual(3, self.environment_model.access_counter)

        try:
            self.test_behavior_true.gain_control(self.time, self.environment_model)
        except Exception as e:
            self.fail(f"gain_control threw an exception: {e}")
        self.assertEqual(4, self.environment_model.access_counter)

        try:
            self.test_behavior_true.lose_control(self.time, self.environment_model)
        except Exception as e:
            self.fail(f"lose_control threw an exception: {e}")
        self.assertEqual(5, self.environment_model.access_counter)

    def test_printout(self):
        ps = PrintStrings()
        expected_printout = ps.invocation_true + ps.commitment_true + "dummy_behavior"
        actual_printout = self.test_behavior_true.to_string(
            self.time, self.environment_model
        )
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        yaml_node = self.test_behavior_true.to_yaml(self.time, self.environment_model)

        self.assertEqual("Behavior", yaml_node["type"])
        self.assertEqual("dummy_behavior", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])
