from arbitration_graphs import CostEstimator


class CostEstimatorFromCostMap(CostEstimator):
    def __init__(self, cost_map, activation_costs=0):
        super().__init__()
        self.cost_map = cost_map
        self.activation_costs = activation_costs

    def estimate_cost(self, time, environment_model, command, is_active):
        # In an actual application, you might want to get data from the environment model here to help you determine
        # the cost of a command. In this mock implementation, we simply simulate this access to show that we could do
        # so.
        environment_model.get_observation()
        if is_active:
            return self.cost_map[command] / (1 + self.activation_costs)
        else:
            return (self.cost_map[command] + self.activation_costs) / (
                1 + self.activation_costs
            )
