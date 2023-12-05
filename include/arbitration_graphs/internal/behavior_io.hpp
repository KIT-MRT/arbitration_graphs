#pragma once

#include "../behavior.hpp"


namespace arbitration_graphs {

template <typename CommandT>
std::string Behavior<CommandT>::to_str(const Time& time, const std::string& prefix, const std::string& suffix) const {
    std::stringstream ss;
    to_stream(ss, time, prefix, suffix);
    return ss.str();
}

template <typename CommandT>
std::ostream& Behavior<CommandT>::to_stream(std::ostream& output,
                                            const Time& time,
                                            const std::string& prefix,
                                            const std::string& suffix) const {
    if (checkInvocationCondition(time)) {
        output << "\033[32mINVOCATION\033[39m ";
    } else {
        output << "\033[31mInvocation\033[39m ";
    }
    if (checkCommitmentCondition(time)) {
        output << "\033[32mCOMMITMENT\033[39m ";
    } else {
        output << "\033[31mCommitment\033[39m ";
    }

    output << name_;
    return output;
}

template <typename CommandT>
YAML::Node Behavior<CommandT>::toYaml(const Time& time) const {
    YAML::Node node;
    node["type"] = "Behavior";
    node["name"] = name_;
    node["invocationCondition"] = checkInvocationCondition(time);
    node["commitmentCondition"] = checkCommitmentCondition(time);
    return node;
}

} // namespace arbitration_graphs