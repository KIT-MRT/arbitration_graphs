# pyright: reportUninitializedInstanceVariable=false

import time
import unittest
from typing import final

import arbitration_graphs as ag

from .cost_estimator import CostEstimatorFromCostMap
from .dummy_types import BrokenDummyBehavior, DummyBehavior, DummyEnvironmentModel


@final
class TestExceptionHandling(unittest.TestCase):
    def test_exception_in_priority_arbitrator(self):
        test_behavior_high_priority = BrokenDummyBehavior(True, True, "HighPriority")
        test_behavior_low_priority = DummyBehavior(True, True, "LowPriority")

        environment_model = DummyEnvironmentModel()

        t = time.time()

        test_priority_arbitrator = ag.PriorityArbitrator()
        test_priority_arbitrator.add_option(
            test_behavior_high_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )
        test_priority_arbitrator.add_option(
            test_behavior_low_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )

        self.assertTrue(
            test_priority_arbitrator.check_invocation_condition(t, environment_model)
        )

        test_priority_arbitrator.gain_control(t, environment_model)

        # Since the high priority behavior is broken, we should get the low priority behavior as fallback
        self.assertEqual(
            "LowPriority", test_priority_arbitrator.get_command(t, environment_model)
        )
        self.assertTrue(test_priority_arbitrator.options()[0].verification_result(t))
        self.assertTrue(test_priority_arbitrator.options()[1].verification_result(t))

        self.assertFalse(
            test_priority_arbitrator.options()[0].verification_result(t).is_ok()
        )
        self.assertTrue(
            test_priority_arbitrator.options()[1].verification_result(t).is_ok()
        )

        test_priority_arbitrator.lose_control(t, environment_model)

        test_behavior_low_priority.invocation_condition = False
        self.assertTrue(
            test_priority_arbitrator.check_invocation_condition(t, environment_model)
        )

        test_priority_arbitrator.gain_control(t, environment_model)

        with self.assertRaises(ag.NoApplicableOptionPassedVerificationError):
            test_priority_arbitrator.get_command(t, environment_model)

    def test_exception_in_commited_behavior(self):
        # This behavior will only raise on the 2nd invocation
        test_behavior_high_priority = BrokenDummyBehavior(True, True, "HighPriority", 1)
        test_behavior_low_priority = DummyBehavior(True, True, "LowPriority")

        environment_model = DummyEnvironmentModel()

        t = time.time()

        test_priority_arbitrator = ag.PriorityArbitrator()
        test_priority_arbitrator.add_option(
            test_behavior_high_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )
        test_priority_arbitrator.add_option(
            test_behavior_low_priority, ag.PriorityArbitrator.Option.Flags.NO_FLAGS
        )

        self.assertTrue(
            test_priority_arbitrator.check_invocation_condition(t, environment_model)
        )

        test_priority_arbitrator.gain_control(t, environment_model)

        # On the first call, the high priority behavior should be selected like it normally would
        self.assertEqual(
            "HighPriority", test_priority_arbitrator.get_command(t, environment_model)
        )
        self.assertTrue(test_priority_arbitrator.options()[0].verification_result(t))
        self.assertTrue(
            test_priority_arbitrator.options()[0].verification_result(t).is_ok()
        )

        # Progress time to not retrieve cached commands
        t += 1

        # On the second call, the high priority behavior will throw an exception
        # The arbitrator should catch the exception and fall back to the low priority behavior
        self.assertEqual(
            "LowPriority", test_priority_arbitrator.get_command(t, environment_model)
        )
        self.assertTrue(test_priority_arbitrator.options()[0].verification_result(t))
        self.assertTrue(test_priority_arbitrator.options()[1].verification_result(t))

        self.assertFalse(
            test_priority_arbitrator.options()[0].verification_result(t).is_ok()
        )
        self.assertTrue(
            test_priority_arbitrator.options()[1].verification_result(t).is_ok()
        )

    def test_exceptions_in_cost_arbitrator(self):
        test_behavior_low_cost = BrokenDummyBehavior(True, True, "LowCost")
        test_behavior_high_cost = DummyBehavior(True, True, "HighCost")

        environment_model = DummyEnvironmentModel()

        cost_map = {"LowCost": 0, "HighCost": 1}
        cost_estimator = CostEstimatorFromCostMap(cost_map)

        t = time.time()

        test_cost_arbitrator = ag.CostArbitrator()
        test_cost_arbitrator.add_option(
            test_behavior_low_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            cost_estimator,
        )
        test_cost_arbitrator.add_option(
            test_behavior_high_cost,
            ag.CostArbitrator.Option.Flags.NO_FLAGS,
            cost_estimator,
        )

        self.assertTrue(
            test_cost_arbitrator.check_invocation_condition(t, environment_model)
        )

        test_cost_arbitrator.gain_control(t, environment_model)

        # The cost arbitrator calls getCommand of all options to estimate the costs
        # Exceptions during the sorting of the options should be caught and handled
        self.assertEqual(
            "HighCost", test_cost_arbitrator.get_command(t, environment_model)
        )
        self.assertTrue(test_cost_arbitrator.options()[0].verification_result(t))
        self.assertTrue(test_cost_arbitrator.options()[1].verification_result(t))

        self.assertFalse(
            test_cost_arbitrator.options()[0].verification_result(t).is_ok()
        )
        self.assertTrue(
            test_cost_arbitrator.options()[1].verification_result(t).is_ok()
        )
