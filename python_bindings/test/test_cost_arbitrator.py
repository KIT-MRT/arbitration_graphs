import os
import time
import unittest

import arbitration_graphs as ag
from dummy_types import DummyBehavior, DummyCommand, PrintStrings
from cost_estimator import CostEstimatorFromCostMap


class CostArbitratorTest(unittest.TestCase):

    def setUp(self):
        self.test_behavior_low_cost = DummyBehavior(False, False, "low_cost")
        self.test_behavior_mid_cost = DummyBehavior(True, False, "mid_cost")
        self.test_behavior_high_cost = DummyBehavior(True, True, "high_cost")

        self.cost_map = {
            DummyCommand("low_cost"): 0,
            DummyCommand("mid_cost"): 0.5,
            DummyCommand("high_cost"): 1,
        }
        self.cost_estimator = CostEstimatorFromCostMap(self.cost_map)
        self.cost_estimator_with_activation_costs = CostEstimatorFromCostMap(
            self.cost_map, 10
        )

        self.test_cost_arbitrator = ag.CostArbitrator()

        self.time = time.time()

    def test_basic_functionality(self):
        # if there are no options yet -> the invocationCondition should be false
        self.assertFalse(
            self.test_cost_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        # Adding options
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        self.test_cost_arbitrator.add_option(
            self.test_behavior_high_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_mid_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )

        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        self.test_cost_arbitrator.gain_control(self.time)
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual(1, self.test_behavior_mid_cost.lose_control_counter)

        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual(3, self.test_behavior_mid_cost.lose_control_counter)

        self.test_behavior_mid_cost.invocation_condition = False
        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertTrue(self.test_cost_arbitrator.check_commitment_condition(self.time))

        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual(3, self.test_behavior_high_cost.lose_control_counter)

        self.test_behavior_mid_cost.invocation_condition = True
        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertTrue(self.test_cost_arbitrator.check_commitment_condition(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))

    def test_printout(self):
        # Adding options
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
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
        self.test_cost_arbitrator.add_option(
            self.test_behavior_mid_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )

        # fmt: off
        ps = PrintStrings()
        expected_printout = (
            ps.invocation_true + ps.commitment_false + "CostArbitrator\n"
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "low_cost\n"
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "low_cost\n"
            "    - (cost:  n.a.) " + ps.invocation_true + ps.commitment_true + "high_cost\n"
            "    - (cost:  n.a.) " + ps.invocation_true + ps.commitment_false + "mid_cost"
        )
        # fmt: on
        actual_printout = self.test_cost_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

        self.test_cost_arbitrator.gain_control(self.time)
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))

        # fmt: off
        expected_printout = (
            ps.invocation_true + ps.commitment_true + "CostArbitrator\n"
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "low_cost\n"
            "    - (cost:  n.a.) " + ps.invocation_false + ps.commitment_false + "low_cost\n"
            "    - (cost: 1.000) " + ps.invocation_true + ps.commitment_true + "high_cost\n"
            " -> - (cost: 0.500) " + ps.invocation_true + ps.commitment_false + "mid_cost"
        )
        # fmt: on
        actual_printout = self.test_cost_arbitrator.to_str(self.time)
        print(actual_printout)

        self.assertEqual(expected_printout, actual_printout)

    def test_to_yaml(self):
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
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
        self.test_cost_arbitrator.add_option(
            self.test_behavior_mid_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            self.cost_estimator,
        )

        yaml_node = self.test_cost_arbitrator.to_yaml(self.time)

        self.assertEqual("CostArbitrator", yaml_node["type"])
        self.assertEqual("CostArbitrator", yaml_node["name"])
        self.assertTrue(yaml_node["invocationCondition"])
        self.assertFalse(yaml_node["commitmentCondition"])

        self.assertEqual(4, len(yaml_node["options"]))
        self.assertEqual("Option", yaml_node["options"][0]["type"])
        self.assertEqual("Option", yaml_node["options"][1]["type"])
        self.assertEqual("Option", yaml_node["options"][2]["type"])
        self.assertEqual("Option", yaml_node["options"][3]["type"])
        self.assertEqual("low_cost", yaml_node["options"][0]["behavior"]["name"])
        self.assertEqual("low_cost", yaml_node["options"][1]["behavior"]["name"])
        self.assertEqual("high_cost", yaml_node["options"][2]["behavior"]["name"])
        self.assertEqual("mid_cost", yaml_node["options"][3]["behavior"]["name"])

        self.assertFalse("flags" in yaml_node["options"][0])
        self.assertFalse("flags" in yaml_node["options"][1])
        self.assertFalse("flags" in yaml_node["options"][2])
        self.assertFalse("flags" in yaml_node["options"][3])
        self.assertFalse("cost" in yaml_node["options"][0])
        self.assertFalse("cost" in yaml_node["options"][1])
        self.assertFalse("cost" in yaml_node["options"][2])
        self.assertFalse("cost" in yaml_node["options"][3])

        self.assertFalse("activeBehavior" in yaml_node)

        self.test_cost_arbitrator.gain_control(self.time)
        self.test_cost_arbitrator.get_command(self.time)

        yaml_node = self.test_cost_arbitrator.to_yaml(self.time)

        self.assertTrue(yaml_node["invocationCondition"])
        self.assertTrue(yaml_node["commitmentCondition"])

        self.assertFalse("cost" in yaml_node["options"][0])
        self.assertFalse("cost" in yaml_node["options"][1])
        self.assertTrue("cost" in yaml_node["options"][2])
        self.assertTrue("cost" in yaml_node["options"][3])
        self.assertAlmostEqual(1.0, yaml_node["options"][2]["cost"], delta=1e-3)
        self.assertAlmostEqual(0.5, yaml_node["options"][3]["cost"], delta=1e-3)

        self.assertTrue("activeBehavior" in yaml_node)
        self.assertEqual(3, yaml_node["activeBehavior"])

    def test_basic_functionality_with_interruptable_options_and_activation_costs(self):
        # If there are no options yet, the invocationCondition should be false
        self.assertFalse(
            self.test_cost_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        # InvocationCondition is true if any option has true invocationCondition
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator_with_activation_costs,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator_with_activation_costs,
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        self.test_cost_arbitrator.add_option(
            self.test_behavior_high_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator_with_activation_costs,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_mid_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator_with_activation_costs,
        )

        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        self.test_cost_arbitrator.gain_control(self.time)
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))

        self.test_behavior_mid_cost.invocation_condition = False
        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertTrue(self.test_cost_arbitrator.check_commitment_condition(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))

        # high_cost behavior is not interruptable -> high_cost should stay active
        self.test_behavior_mid_cost.invocation_condition = True
        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertTrue(self.test_cost_arbitrator.check_commitment_condition(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))

    def test_basic_functionality_with_interruptable_options(self):
        # If there are no options yet, the invocationCondition should be false
        self.assertFalse(
            self.test_cost_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        # InvocationCondition is true if any option has true invocationCondition
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator,
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_invocation_condition(self.time)
        )
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        self.test_cost_arbitrator.add_option(
            self.test_behavior_high_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator,
        )
        self.test_cost_arbitrator.add_option(
            self.test_behavior_mid_cost,
            ag.CostArbitrator.Option.Flags.INTERRUPTABLE,
            self.cost_estimator,
        )

        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertFalse(
            self.test_cost_arbitrator.check_commitment_condition(self.time)
        )

        self.test_cost_arbitrator.gain_control(self.time)
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))

        self.test_behavior_mid_cost.invocation_condition = False
        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertTrue(self.test_cost_arbitrator.check_commitment_condition(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual("high_cost", self.test_cost_arbitrator.get_command(self.time))

        # high_cost behavior is interruptable -> mid_cost should become active again
        self.test_behavior_mid_cost.invocation_condition = True
        self.assertTrue(self.test_cost_arbitrator.check_invocation_condition(self.time))
        self.assertTrue(self.test_cost_arbitrator.check_commitment_condition(self.time))
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))
        self.assertEqual("mid_cost", self.test_cost_arbitrator.get_command(self.time))
