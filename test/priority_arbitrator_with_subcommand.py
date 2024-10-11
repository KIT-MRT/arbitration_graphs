import sys
import os

build_directory = "/home/blinky/arbitration_graphs/build/"
sys.path.append(build_directory)

import time
import unittest

import arbitration_graphs_py_with_subcommand as ag_subcommand


class TestPriorityArbitratorWithSubCommand(unittest.TestCase):
    def test_sub_command_type_differs_from_command_type(self):
        now = time.time()

        # We cannot mix arbitration_graphs_py_with_subcommand and arbitration_graphs_py since they bind some of the same types.
        # To not create a another DummyBehavior implementation, we'll just use the pybind types for this test.
        test_priority_arbitrator = ag_subcommand.PriorityArbitrator(
            "test_arbitrator_with_subcommand"
        )
        test_behavior_high_priority = ag_subcommand.testing_types.DummyBehavior(
            False, False, "___HighPriority___"
        )
        test_behavior_mid_priority = ag_subcommand.testing_types.DummyBehavior(
            True, False, "__MidPriority__"
        )
        test_behavior_low_priority = ag_subcommand.testing_types.DummyBehavior(
            True, True, "_LowPriority_"
        )

        test_priority_arbitrator.add_option(
            test_behavior_high_priority,
            ag_subcommand.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            test_behavior_high_priority,
            ag_subcommand.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            test_behavior_mid_priority,
            ag_subcommand.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )
        test_priority_arbitrator.add_option(
            test_behavior_low_priority,
            ag_subcommand.PriorityArbitrator.Option.Flags.NO_FLAGS,
        )

        test_priority_arbitrator.gain_control(now)

        expected = "__MidPriority__"
        self.assertEqual(len(expected), test_priority_arbitrator.get_command(now))


if __name__ == "__main__":
    unittest.main()
