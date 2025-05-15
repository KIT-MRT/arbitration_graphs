from arbitration_graphs import CostEstimator


class CostEstimatorFromCostMap(CostEstimator):
    def __init__(self, cost_map, activation_costs=0):
        super().__init__()
        self.cost_map = cost_map
        self.activation_costs = activation_costs

    def estimate_cost(self, command, is_active):
        if is_active:
            return self.cost_map[command] / (1 + self.activation_costs)
        else:
            return (self.cost_map[command] + self.activation_costs) / (
                1 + self.activation_costs
            )
