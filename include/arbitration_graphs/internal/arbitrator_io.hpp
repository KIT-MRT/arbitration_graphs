#pragma once

#include "../arbitrator.hpp"


namespace arbitration_graphs {

//////////////////////////////
//    Arbitrator::Option    //
//////////////////////////////

template <typename CommandT, typename SubCommandT>
std::ostream& Arbitrator<CommandT, SubCommandT>::Option::to_stream(std::ostream& output,
                                                                   const Time& time,
                                                                   const int& option_index,
                                                                   const std::string& prefix,
                                                                   const std::string& suffix) const {

    if (verificationResult_.cached(time) && !verificationResult_.cached(time).value()->isOk()) {
        // ANSI backspace: \010
        // ANSI strikethrough on: \033[9m
        output << "×××\010\010\010\033[9m";
        behavior_->to_stream(output, time, prefix, suffix);
        // ANSI strikethrough off: \033[29m
        // ANSI hide on: \033[8m
        // ANSI hide off: \033[28m
        output << "\033[29m\033[8m×××\033[28m";
    } else {
        behavior_->to_stream(output, time, prefix, suffix);
    }

    return output;
}

template <typename CommandT, typename SubCommandT>
YAML::Node Arbitrator<CommandT, SubCommandT>::Option::toYaml(const Time& time) const {
    YAML::Node node;
    node["type"] = "Option";
    node["behavior"] = behavior_->toYaml(time);
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

template <typename CommandT, typename SubCommandT>
std::ostream& Arbitrator<CommandT, SubCommandT>::to_stream(std::ostream& output,
                                                           const Time& time,
                                                           const std::string& prefix,
                                                           const std::string& suffix) const {

    Behavior<CommandT>::to_stream(output, time, prefix, suffix);

    for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
        const typename Option::Ptr& option = behaviorOptions_.at(i);
        bool isActive = activeBehavior_ && (option == activeBehavior_);

        if (isActive) {
            output << suffix << std::endl << prefix << " -> ";
        } else {
            output << suffix << std::endl << prefix << "    ";
        }
        option->to_stream(output, time, i, "    " + prefix, suffix);
    }
    return output;
}

template <typename CommandT, typename SubCommandT>
YAML::Node Arbitrator<CommandT, SubCommandT>::toYaml(const Time& time) const {
    YAML::Node node = Behavior<CommandT>::toYaml(time);

    node["type"] = "Arbitrator";
    for (const typename Option::Ptr& option : behaviorOptions_) {
        YAML::Node yaml = option->toYaml(time);
        node["options"].push_back(yaml);
    }
    if (activeBehavior_) {
        node["activeBehavior"] = getOptionIndex(activeBehavior_);
    }

    return node;
}

} // namespace arbitration_graphs
