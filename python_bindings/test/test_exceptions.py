import os
import time
import unittest

import arbitration_graphs as ag

from dummy_types import BrokenDummyBehavior, DummyBehavior


class TestExceptionHandling(unittest.TestCase):
    def test_exception_in_priority_arbitrator(self):
        test_behavior_high_priority = BrokenDummyBehavior(True, True, "HighPriority")
        test_behavior_low_priority = DummyBehavior(True, True, "LowPriority")

        t = time.time()

        test_priority_arbitrator = ag.PriorityArbitrator()
        test_priority_arbitrator.add_option(
            test_behavior_high_priority, ag.PriorityArbitrator.Option.NO_FLAGS
        )
        test_priority_arbitrator.add_option(
            test_behavior_low_priority, ag.PriorityArbitrator.Option.NO_FLAGS
        )

        self.assertTrue(test_priority_arbitrator.check_invocation_condition(t))

        test_priority_arbitrator.gain_control(t)

        # Since the high priority behavior is broken, we should get the low priority behavior as fallback
        self.assertEqual("LowPriority", test_priority_arbitrator.get_command(t))
        self.assertTrue(
            test_priority_arbitrator.options()[0].verification_result.cached(t)
        )
        self.assertTrue(
            test_priority_arbitrator.options()[1].verification_result.cached(t)
        )

        self.assertFalse(
            test_priority_arbitrator.options()[0].verification_result.cached(t).is_ok()
        )
        self.assertTrue(
            test_priority_arbitrator.options()[1].verification_result.cached(t).is_ok()
        )

        test_priority_arbitrator.lose_control(t)

        test_behavior_low_priority.invocation_condition = False
        self.assertTrue(test_priority_arbitrator.check_invocation_condition(t))

        test_priority_arbitrator.gain_control(t)

        with self.assertRaises(ag.NoApplicableOptionPassedVerificationError):
            test_priority_arbitrator.get_command(t)
