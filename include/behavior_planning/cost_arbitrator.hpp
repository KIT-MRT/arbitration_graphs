#pragma once

#include <iomanip>
#include <memory>
#include <optional>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"


namespace behavior_planning {

template <typename SubCommandT>
struct CostEstimator {
    using Ptr = std::shared_ptr<CostEstimator>;
    using ConstPtr = std::shared_ptr<const CostEstimator>;

    virtual double estimateCost(const SubCommandT& command, const bool isActive) = 0;
};

template <typename CommandT, typename SubCommandT = CommandT>
class CostArbitrator : public Arbitrator<CommandT, SubCommandT> {
public:
    using Ptr = std::shared_ptr<CostArbitrator>;
    using ConstPtr = std::shared_ptr<const CostArbitrator>;

    struct Option : Arbitrator<CommandT, SubCommandT>::Option {
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename Arbitrator<CommandT, SubCommandT>::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior,
               const FlagsT& flags,
               const typename CostEstimator<SubCommandT>::Ptr& costEstimator)
                : Arbitrator<CommandT, SubCommandT>::Option(behavior, flags), costEstimator_{costEstimator} {
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output        Output stream to write into, will be returned also
         * \param time          Expected execution time point of this behaviors command
         * \param option_index  Position index of this option within behaviorOptions_
         * \param prefix        A string that should be prepended to each line that is written to the output stream
         * \param suffix        A string that should be appended to each line that is written to the output stream
         * \return              The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::to_stream()
         */
        virtual std::ostream& to_stream(std::ostream& output,
                                        const Time& time,
                                        const int& option_index,
                                        const std::string& prefix = "",
                                        const std::string& suffix = "") const {
            if (last_estimated_cost_) {
                output << std::fixed << std::setprecision(3) << "- (cost: " << *last_estimated_cost_ << ") ";
            } else {
                output << "- (cost:  n.a.) ";
            }

            Arbitrator<CommandT, SubCommandT>::Option::to_stream(output, time, option_index, prefix, suffix);
            return output;
        }

        /*!
         * \brief Returns a yaml representation of this option with its current state
         *
         * \param time  Expected execution time point of this behaviors command
         * \return      Yaml representation of this behavior
         */
        virtual YAML::Node toYaml(const Time& time) const override {
            YAML::Node node = Arbitrator<CommandT, SubCommandT>::Option::toYaml(time);
            if (last_estimated_cost_) {
                node["cost"] = *last_estimated_cost_;
            }
            return node;
        }

        typename CostEstimator<SubCommandT>::Ptr costEstimator_;
        mutable std::optional<double> last_estimated_cost_;
    };


    CostArbitrator(const std::string& name = "CostArbitrator") : Arbitrator<CommandT, SubCommandT>(name){};


    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior,
                   const typename Option::Flags& flags,
                   const typename CostEstimator<SubCommandT>::Ptr& costEstimator) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags, costEstimator);
        this->behaviorOptions_.push_back(option);
    }

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      Yaml representation of this behavior
     */
    virtual YAML::Node toYaml(const Time& time) const override {
        YAML::Node node = Arbitrator<CommandT, SubCommandT>::toYaml(time);

        node["type"] = "CostArbitrator";
        node["options"] = YAML::Null;
        for (const auto& option : this->behaviorOptions_) {
            node["options"].push_back(option->toYaml(time));
        }

        return node;
    }

private:
    /*!
     * Find behavior option with lowest cost and true invocation condition
     *
     * @return  Applicable option with lowest costs (can also be the currently active option)
     */
    typename Arbitrator<CommandT, SubCommandT>::Option::Ptr findBestOption(const Time& time) const override {
        double costOfBestOption = std::numeric_limits<double>::max();
        typename Option::Ptr bestOption;

        for (auto& option_base : this->behaviorOptions_) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(option_base);

            bool isActive = this->activeBehavior_ && (option == this->activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && this->activeBehavior_->behavior_->checkCommitmentCondition(time);
            if (option->behavior_->checkInvocationCondition(time) || isActiveAndCanBeContinued) {
                double cost;
                if (isActive) {
                    cost = option->costEstimator_->estimateCost(option->behavior_->getCommand(time), isActive);
                } else {
                    option->behavior_->gainControl(time);
                    cost = option->costEstimator_->estimateCost(option->behavior_->getCommand(time), isActive);
                    option->behavior_->loseControl(time);
                }
                option->last_estimated_cost_ = cost;

                if (cost < costOfBestOption) {
                    costOfBestOption = cost;
                    bestOption = option;
                }
            } else {
                option->last_estimated_cost_ = std::nullopt;
            }
        }
        return bestOption;
    }
};
} // namespace behavior_planning
