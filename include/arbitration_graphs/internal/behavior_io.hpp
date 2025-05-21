#pragma once

#include "../behavior.hpp"


namespace arbitration_graphs {

template <typename CommandT, typename EnvironmentModelT>
std::string Behavior<CommandT, EnvironmentModelT>::to_str(const Time& time,
                                                          const EnvironmentModelT& environmentModel,
                                                          const std::string& prefix,
                                                          const std::string& suffix) const {
    std::stringstream ss;
    to_stream(ss, time, environmentModel, prefix, suffix);
    return ss.str();
}

template <typename CommandT, typename EnvironmentModelT>
std::ostream& Behavior<CommandT, EnvironmentModelT>::to_stream(std::ostream& output,
                                                               const Time& time,
                                                               const EnvironmentModelT& environmentModel,
                                                               const std::string& prefix,
                                                               const std::string& suffix) const {
    if (checkInvocationCondition(time, environmentModel)) {
        output << "\033[32mINVOCATION\033[39m ";
    } else {
        output << "\033[31mInvocation\033[39m ";
    }
    if (checkCommitmentCondition(time, environmentModel)) {
        output << "\033[32mCOMMITMENT\033[39m ";
    } else {
        output << "\033[31mCommitment\033[39m ";
    }

    output << name_;
    return output;
}

template <typename CommandT, typename EnvironmentModelT>
YAML::Node Behavior<CommandT, EnvironmentModelT>::toYaml(const Time& time,
                                                         const EnvironmentModelT& environmentModel) const {
    YAML::Node node;
    node["type"] = "Behavior";
    node["name"] = name_;
    node["invocationCondition"] = checkInvocationCondition(time, environmentModel);
    node["commitmentCondition"] = checkCommitmentCondition(time, environmentModel);
    return node;
}

} // namespace arbitration_graphs
