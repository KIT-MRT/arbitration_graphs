import os
import time
import unittest
import yaml

import arbitration_graphs as ag

from dummy_types import BrokenDummyBehavior, DummyBehavior


class TestExceptionHandling(unittest.TestCase):

    def setUp(self):
        self.test_behavior_high_priority = BrokenDummyBehavior(
            True, True, "HighPriority"
        )
        self.test_behavior_low_priority = DummyBehavior(True, True, "LowPriority")

        self.time = time.time()

    def test_handle_exception(self):
        test_priority_arbitrator = ag.PriorityArbitrator()
        test_priority_arbitrator.add_option(
            self.test_behavior_high_priority, ag.PriorityArbitrator.Option.NO_FLAGS
        )
        test_priority_arbitrator.add_option(
            self.test_behavior_low_priority, ag.PriorityArbitrator.Option.NO_FLAGS
        )

        self.assertTrue(test_priority_arbitrator.check_invocation_condition(self.time))

        test_priority_arbitrator.gain_control(self.time)

        # Since the high priority behavior is broken, we should get the low priority behavior as fallback
        self.assertEqual("LowPriority", test_priority_arbitrator.get_command(self.time))
        self.assertTrue(
            test_priority_arbitrator.options()[0].verification_result.cached(self.time)
        )
        self.assertTrue(
            test_priority_arbitrator.options()[1].verification_result.cached(self.time)
        )

        self.assertFalse(
            test_priority_arbitrator.options()[0]
            .verification_result.cached(self.time)
            .is_ok()
        )
        self.assertTrue(
            test_priority_arbitrator.options()[1]
            .verification_result.cached(self.time)
            .is_ok()
        )

        test_priority_arbitrator.lose_control(self.time)

        self.test_behavior_low_priority.invocation_condition = False
        self.assertTrue(test_priority_arbitrator.check_invocation_condition(self.time))

        test_priority_arbitrator.gain_control(self.time)

        with self.assertRaises(ag.NoApplicableOptionPassedVerificationError):
            test_priority_arbitrator.get_command(self.time)


if __name__ == "__main__":
    header = "Running " + os.path.basename(__file__)

    print("=" * len(header))
    print(header)
    print("=" * len(header) + "\n")
    unittest.main(exit=False)
    print("=" * len(header) + "\n")
