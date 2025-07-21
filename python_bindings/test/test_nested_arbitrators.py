# pyright: reportUninitializedInstanceVariable=false

import time
import unittest
from typing import final

from typing_extensions import override

import arbitration_graphs as ag

from .cost_estimator import CostEstimatorFromCostMap
from .dummy_types import DummyBehavior, DummyEnvironmentModel, PrintStrings


@final
class NestedArbitratorsTest(unittest.TestCase):
    @override
    def setUp(self):
        self.test_behavior_high_priority = DummyBehavior(False, False, "HighPriority")
        self.test_behavior_low_priority = DummyBehavior(True, True, "LowPriority")

        self.test_behavior_low_cost = DummyBehavior(False, False, "low_cost")
        self.test_behavior_mid_cost = DummyBehavior(True, False, "mid_cost")
        self.test_behavior_high_cost = DummyBehavior(True, True, "high_cost")

        cost_map = {"low_cost": 0, "mid_cost": 0.5, "high_cost": 1}
        self.cost_estimator = CostEstimatorFromCostMap(cost_map)

        self.test_cost_arbitrator = ag.CostArbitrator()
        self.test_priority_arbitrator = ag.PriorityArbitrator()

        self.test_root_priority_arbitrator = ag.PriorityArbitrator(
            "root priority arbitrator"
        )

        self.environment_model = DummyEnvironmentModel()

        self.time = time.time()

    def test_printout(self):
        self.test_root_priority_arbitrator.add_option(
            self.test_cost_arbitrator,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_root_priority_arbitrator.add_option(
            self.test_priority_arbitrator,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_high_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )

        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_low_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        # fmt: off
        ps = PrintStrings()
        expected_printout = (
            ps.invocation_true + ps.commitment_false + "root priority arbitrator\n" +
            "    1. " + ps.invocation_true + ps.commitment_false + "CostArbitrator\n" +
            "        - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "low_cost\n" +
            "        - (cost:  n.a.) " + ps.invocation_true + ps.commitment_true + "high_cost\n" +
            "    2. " + ps.invocation_true + ps.commitment_false + "PriorityArbitrator\n" +
            "        1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "        2. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt: on

        actual_printout = self.test_root_priority_arbitrator.to_string(
            self.time, self.environment_model
        )
        self.assertEqual(expected_printout, actual_printout)
        print(actual_printout)

        self.test_priority_arbitrator.gain_control(self.time, self.environment_model)
        self.assertEqual(
            "high_cost",
            self.test_root_priority_arbitrator.get_command(
                self.time, self.environment_model
            ),
        )

        # fmt: off
        expected_printout_after_command = (
            ps.invocation_true + ps.commitment_true + "root priority arbitrator\n" +
            " -> 1. " + ps.invocation_true + ps.commitment_true + "CostArbitrator\n" +
            "        - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "low_cost\n" +
            "     -> - (cost: 1.000) " + ps.invocation_true + ps.commitment_true + "high_cost\n" +
            "    2. " + ps.invocation_true + ps.commitment_false + "PriorityArbitrator\n" +
            "        1. " + ps.invocation_false + ps.commitment_false + "HighPriority\n" +
            "        2. " + ps.invocation_true + ps.commitment_true + "LowPriority"
        )
        # fmt: on

        actual_printout = self.test_root_priority_arbitrator.to_string(
            self.time, self.environment_model
        )
        self.assertEqual(expected_printout_after_command, actual_printout)
        print(actual_printout)

    def test_to_yaml(self):
        self.test_root_priority_arbitrator.add_option(
            self.test_cost_arbitrator,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_root_priority_arbitrator.add_option(
            self.test_priority_arbitrator,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_high_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )

        self.test_priority_arbitrator.add_option(
            self.test_behavior_high_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        self.test_priority_arbitrator.add_option(
            self.test_behavior_low_priority,
            ag.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        yaml_node = self.test_root_priority_arbitrator.to_yaml(
            self.time, self.environment_model
        )

        self.assertEqual("PriorityArbitrator", yaml_node["type"])
        self.assertEqual("root priority arbitrator", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertFalse(yaml_node["commitmentCondition"])

        self.assertEqual(2, len(yaml_node["options"]))
        self.assertEqual("Option", yaml_node["options"][0]["type"])
        self.assertEqual("Option", yaml_node["options"][1]["type"])
        self.assertEqual("CostArbitrator", yaml_node["options"][0]["behavior"]["name"])
        self.assertEqual(
            "PriorityArbitrator", yaml_node["options"][1]["behavior"]["name"]
        )
        self.assertFalse("flags" in yaml_node["options"][0])
        self.assertFalse("flags" in yaml_node["options"][1])

        self.assertEqual(2, len(yaml_node["options"][0]["behavior"]["options"]))
        self.assertFalse("activeBehavior" in yaml_node["options"][0]["behavior"])
        self.assertEqual(
            "low_cost",
            yaml_node["options"][0]["behavior"]["options"][0]["behavior"]["name"],
        )
        self.assertEqual(
            "high_cost",
            yaml_node["options"][0]["behavior"]["options"][1]["behavior"]["name"],
        )

        self.assertEqual(2, len(yaml_node["options"][1]["behavior"]["options"]))
        self.assertFalse("activeBehavior" in yaml_node["options"][1]["behavior"])
        self.assertEqual(
            "HighPriority",
            yaml_node["options"][1]["behavior"]["options"][0]["behavior"]["name"],
        )
        self.assertEqual(
            "LowPriority",
            yaml_node["options"][1]["behavior"]["options"][1]["behavior"]["name"],
        )

        self.assertFalse("activeBehavior" in yaml_node)

        self.test_priority_arbitrator.gain_control(self.time, self.environment_model)
        self.test_root_priority_arbitrator.get_command(
            self.time, self.environment_model
        )

        yaml_node = self.test_root_priority_arbitrator.to_yaml(
            self.time, self.environment_model
        )

        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])

        self.assertTrue("activeBehavior" in yaml_node)
        self.assertEqual(0, yaml_node["activeBehavior"])

        self.assertTrue("activeBehavior" in yaml_node["options"][0]["behavior"])
        self.assertEqual(1, yaml_node["options"][0]["behavior"]["activeBehavior"])
