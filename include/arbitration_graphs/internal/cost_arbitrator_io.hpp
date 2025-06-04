#pragma once

#include "../cost_arbitrator.hpp"


namespace arbitration_graphs {

//////////////////////////////////
//    CostArbitrator::Option    //
//////////////////////////////////

template <typename EnvironmentModelT,
          typename CommandT,
          typename SubCommandT,
          typename VerifierT,
          typename VerificationResultT>
std::ostream& CostArbitrator<EnvironmentModelT, CommandT, SubCommandT, VerifierT, VerificationResultT>::Option::
    to_stream(std::ostream& output,
              const Time& time,
              const EnvironmentModelT& environmentModel,
              const int& option_index,
              const std::string& prefix,
              const std::string& suffix) const {

    if (last_estimated_cost_) {
        output << std::fixed << std::setprecision(3) << "- (cost: " << *last_estimated_cost_ << ") ";
    } else {
        output << "- (cost:  n.a.) ";
    }

    ArbitratorBase::Option::to_stream(output, time, environmentModel, option_index, prefix, suffix);
    return output;
}

template <typename EnvironmentModelT,
          typename CommandT,
          typename SubCommandT,
          typename VerifierT,
          typename VerificationResultT>
YAML::Node CostArbitrator<EnvironmentModelT, CommandT, SubCommandT, VerifierT, VerificationResultT>::Option::toYaml(
    const Time& time, const EnvironmentModelT& environmentModel) const {
    YAML::Node node = ArbitratorBase::Option::toYaml(time, environmentModel);
    if (last_estimated_cost_) {
        node["cost"] = *last_estimated_cost_;
    }
    return node;
}


//////////////////////////////////
//        CostArbitrator        //
//////////////////////////////////

template <typename EnvironmentModelT,
          typename CommandT,
          typename SubCommandT,
          typename VerifierT,
          typename VerificationResultT>
YAML::Node CostArbitrator<EnvironmentModelT, CommandT, SubCommandT, VerifierT, VerificationResultT>::toYaml(
    const Time& time, const EnvironmentModelT& environmentModel) const {
    YAML::Node node = ArbitratorBase::toYaml(time, environmentModel);

    node["type"] = "CostArbitrator";
    node["options"] = YAML::Null;
    for (const auto& option : this->behaviorOptions_) {
        node["options"].push_back(option->toYaml(time, environmentModel));
    }

    return node;
}

} // namespace arbitration_graphs