import os
import time
import unittest

import arbitration_graphs_py_with_subcommand as ag_subcommand


class TestConjunctiveCoordinatorWithSubCommand(unittest.TestCase):
    def test_sub_command_type_differs_from_command_type(self):
        now = time.time()

        # We cannot mix arbitration_graphs_py_with_subcommand and arbitration_graphs_py since they bind some of the same types.
        # To not create a another DummyBehavior implementation, we'll just use the pybind types for this test.
        test_behavior_a = ag_subcommand.testing_types.DummyBehavior(False, False, "A")
        test_behavior_b1 = ag_subcommand.testing_types.DummyBehavior(True, False, "B")
        test_behavior_c = ag_subcommand.testing_types.DummyBehavior(True, True, "C")
        test_behavior_b2 = ag_subcommand.testing_types.DummyBehavior(True, False, "B")

        test_conjunctive_coordinator = ag_subcommand.ConjunctiveCoordinator()

        test_conjunctive_coordinator.add_option(
            test_behavior_a,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        test_conjunctive_coordinator.add_option(
            test_behavior_b1,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        test_conjunctive_coordinator.add_option(
            test_behavior_c,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        test_conjunctive_coordinator.add_option(
            test_behavior_b2,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )

        test_behavior_a.invocation_condition = True
        test_conjunctive_coordinator.gain_control(now)

        expected = "ABCB"
        self.assertEqual(len(expected), test_conjunctive_coordinator.get_command(now))


class TestCostArbitratorWithSubCommand(unittest.TestCase):
    def test_sub_command_type_differs_from_command_type(self):
        now = time.time()

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


class TestJointCoordinatorWithSubCommand(unittest.TestCase):
    def test_sub_command_type_differs_from_command_type(self):
        now = time.time()

        NO_FLAGS = ag_subcommand.JointCoordinator.Option.Flags.NO_FLAGS

        test_behavior_a = ag_subcommand.testing_types.DummyBehavior(False, False, "A")
        test_behavior_b1 = ag_subcommand.testing_types.DummyBehavior(True, False, "B")
        test_behavior_c = ag_subcommand.testing_types.DummyBehavior(True, True, "C")
        test_behavior_b2 = ag_subcommand.testing_types.DummyBehavior(True, False, "B")

        test_joint_coordinator = ag_subcommand.JointCoordinator()

        test_joint_coordinator.add_option(test_behavior_a, NO_FLAGS)
        test_joint_coordinator.add_option(test_behavior_b1, NO_FLAGS)
        test_joint_coordinator.add_option(test_behavior_c, NO_FLAGS)
        test_joint_coordinator.add_option(test_behavior_b2, NO_FLAGS)

        test_behavior_a.invocation_condition = True
        test_joint_coordinator.gain_control(now)

        expected = "ABCB"

        self.assertEqual(len(expected), test_joint_coordinator.get_command(now))


class TestPriorityArbitratorWithSubCommand(unittest.TestCase):
    def test_sub_command_type_differs_from_command_type(self):
        now = time.time()

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
    header = "Running " + os.path.basename(__file__)

    print("=" * len(header))
    print(header)
    print("=" * len(header) + "\n")
    unittest.main(exit=False)
    print("=" * len(header) + "\n")
