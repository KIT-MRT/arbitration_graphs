import time
import unittest

import arbitration_graphs_py_with_subcommand as ag_subcommand


class TestCostArbitratorWithSubCommand(unittest.TestCase):
    def test_sub_command_type_differs_from_command_type(self):
        now = time.time()

        # We cannot mix arbitration_graphs_py_with_subcommand and arbitration_graphs_py since they bind some of the same types.
        # To not create a another DummyBehavior implementation, we'll just use the pybind types for this test.
        test_behavior_low_cost = ag_subcommand.testing_types.DummyBehavior(
            False, False, "low_cost"
        )
        test_behavior_mid_cost = ag_subcommand.testing_types.DummyBehavior(
            True, False, "__mid_cost__"
        )
        test_behavior_high_cost = ag_subcommand.testing_types.DummyBehavior(
            True, True, "____high_cost____"
        )

        cost_map = {
            "low_cost": 0,
            "__mid_cost__": 0.5,
            "____high_cost____": 1,
        }
        cost_estimator = ag_subcommand.testing_types.CostEstimatorFromCostMap(cost_map)

        test_cost_arbitrator = ag_subcommand.CostArbitrator("test_arbitrator_with_cost")

        test_cost_arbitrator.add_option(
            test_behavior_low_cost,
            ag_subcommand.CostArbitrator.Option.Flags.INTERRUPTABLE,
            cost_estimator,
        )
        test_cost_arbitrator.add_option(
            test_behavior_low_cost,
            ag_subcommand.CostArbitrator.Option.Flags.INTERRUPTABLE,
            cost_estimator,
        )
        test_cost_arbitrator.add_option(
            test_behavior_high_cost,
            ag_subcommand.CostArbitrator.Option.Flags.INTERRUPTABLE,
            cost_estimator,
        )
        test_cost_arbitrator.add_option(
            test_behavior_mid_cost,
            ag_subcommand.CostArbitrator.Option.Flags.INTERRUPTABLE,
            cost_estimator,
        )

        # Gain control with the current time
        test_cost_arbitrator.gain_control(now)

        # Validate the outcome
        expected = "__mid_cost__"
        self.assertEqual(len(expected), test_cost_arbitrator.get_command(now))


if __name__ == "__main__":
    unittest.main()
