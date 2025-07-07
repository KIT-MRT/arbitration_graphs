#pragma once

#include "../arbitrator.hpp"


namespace arbitration_graphs {

//////////////////////////////
//    Arbitrator::Option    //
//////////////////////////////

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::ostream& Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::Option::to_stream(
    std::ostream& output,
    const Time& time,
    const EnvironmentModelT& environmentModel,
    const int& /*optionIndex*/,
    const std::string& prefix,
    const std::string& suffix) const {

    if (verificationResult_.cached(time) && !verificationResult_.cached(time).value()->isOk()) {
        // ANSI backspace: \010
        // ANSI strikethrough on: \033[9m
        output << "×××\010\010\010\033[9m";
        behavior_->to_stream(output, time, environmentModel, prefix, suffix);
        // ANSI strikethrough off: \033[29m
        // ANSI hide on: \033[8m
        // ANSI hide off: \033[28m
        output << "\033[29m\033[8m×××\033[28m";
    } else {
        behavior_->to_stream(output, time, environmentModel, prefix, suffix);
    }

    return output;
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
YAML::Node Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::Option::toYaml(
    const Time& time, const EnvironmentModelT& environmentModel) const {
    YAML::Node node;
    node["type"] = "Option";
    node["behavior"] = behavior_->toYaml(time, environmentModel);
    if (verificationResult_.cached(time)) {
        node["verificationResult"] = verificationResult_.cached(time).value()->isOk() ? "passed" : "failed";
    }
    if (hasFlag(Option::Flags::INTERRUPTABLE)) {
        node["flags"].push_back("INTERRUPTABLE");
    }
    if (hasFlag(Option::Flags::FALLBACK)) {
        node["flags"].push_back("FALLBACK");
    }

    return node;
}


//////////////////////////////
//        Arbitrator        //
//////////////////////////////

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::ostream& Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::to_stream(std::ostream& output,
                                                                              const Time& time,
                                                                              const EnvironmentModelT& environmentModel,
                                                                              const std::string& prefix,
                                                                              const std::string& suffix) const {

    Behavior<EnvironmentModelT, CommandT>::to_stream(output, time, environmentModel, prefix, suffix);

    for (int i = 0; i < static_cast<int>(behaviorOptions_.size()); ++i) {
        const typename Option::Ptr& option = behaviorOptions_.at(i);
        bool isActive = activeBehavior_ && (option == activeBehavior_);

        if (isActive) {
            output << suffix << std::endl << prefix << " -> ";
        } else {
            output << suffix << std::endl << prefix << "    ";
        }
        option->to_stream(output, time, environmentModel, i, "    " + prefix, suffix);
    }
    return output;
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
YAML::Node Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::toYaml(
    const Time& time, const EnvironmentModelT& environmentModel) const {
    YAML::Node node = Behavior<EnvironmentModelT, CommandT>::toYaml(time, environmentModel);

    node["type"] = "Arbitrator";
    for (const typename Option::Ptr& option : behaviorOptions_) {
        YAML::Node yaml = option->toYaml(time, environmentModel);
        node["options"].push_back(yaml);
    }
    if (activeBehavior_) {
        node["activeBehavior"] = getOptionIndex(activeBehavior_);
    }

    return node;
}

} // namespace arbitration_graphs
