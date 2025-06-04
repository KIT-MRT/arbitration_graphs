#pragma once

#include "../priority_arbitrator.hpp"


namespace arbitration_graphs {


//////////////////////////////////////
//    PriorityArbitrator::Option    //
//////////////////////////////////////

template <typename EnvironmentModelT,
          typename CommandT,
          typename SubCommandT,
          typename VerifierT,
          typename VerificationResultT>
std::ostream& PriorityArbitrator<EnvironmentModelT, CommandT, SubCommandT, VerifierT, VerificationResultT>::Option::
    to_stream(std::ostream& output,
              const Time& time,
              const EnvironmentModelT& environmentModel,
              const int& option_index,
              const std::string& prefix,
              const std::string& suffix) const {

    output << option_index + 1 << ". ";
    ArbitratorBase::Option::to_stream(output, time, environmentModel, option_index, prefix, suffix);
    return output;
}


//////////////////////////////////////
//        PriorityArbitrator        //
//////////////////////////////////////

template <typename EnvironmentModelT,
          typename CommandT,
          typename SubCommandT,
          typename VerifierT,
          typename VerificationResultT>
YAML::Node PriorityArbitrator<EnvironmentModelT, CommandT, SubCommandT, VerifierT, VerificationResultT>::toYaml(
    const Time& time, const EnvironmentModelT& environmentModel) const {
    YAML::Node node = ArbitratorBase::toYaml(time, environmentModel);
    node["type"] = "PriorityArbitrator";
    return node;
}

} // namespace arbitration_graphs