#pragma once

#include <algorithm>
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

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>>
class CostArbitrator : public Arbitrator<CommandT, SubCommandT, VerifierT> {
public:
    using ArbitratorBase = Arbitrator<CommandT, SubCommandT, VerifierT>;

    using Ptr = std::shared_ptr<CostArbitrator>;
    using ConstPtr = std::shared_ptr<const CostArbitrator>;

    struct Option : ArbitratorBase::Option {
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior,
               const FlagsT& flags,
               const typename CostEstimator<SubCommandT>::Ptr& costEstimator)
                : ArbitratorBase::Option(behavior, flags), costEstimator_{costEstimator} {
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

            ArbitratorBase::Option::to_stream(output, time, option_index, prefix, suffix);
            return output;
        }

        /*!
         * \brief Returns a yaml representation of this option with its current state
         *
         * \param time  Expected execution time point of this behaviors command
         * \return      Yaml representation of this behavior
         */
        virtual YAML::Node toYaml(const Time& time) const override {
            YAML::Node node = ArbitratorBase::Option::toYaml(time);
            if (last_estimated_cost_) {
                node["cost"] = *last_estimated_cost_;
            }
            return node;
        }

        typename CostEstimator<SubCommandT>::Ptr costEstimator_;
        mutable std::optional<double> last_estimated_cost_;
    };


    CostArbitrator(const std::string& name = "CostArbitrator") : ArbitratorBase(name){};


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
        YAML::Node node = ArbitratorBase::toYaml(time);

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
    typename ArbitratorBase::Options sortOptionsByGivenPolicy(const typename ArbitratorBase::Options& options,
                                                              const Time& time) const override {
        // reset last_estimated_cost_ for all behaviorOptions_
        for (const auto& optionBase : this->behaviorOptions_) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(optionBase);
            option->last_estimated_cost_ = std::nullopt;
        }

        // sort given options by using a multiset
        std::multimap<double, typename ArbitratorBase::Option::Ptr> sortedOptionsMap;

        for (auto& optionBase : options) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(optionBase);

            const bool isActive = this->isActive(option);

            option->behavior_->gainControl(time);
            const double cost = option->costEstimator_->estimateCost(option->behavior_->getCommand(time), isActive);
            option->behavior_->loseControl(time);

            option->last_estimated_cost_ = cost;
            sortedOptionsMap.insert({cost, option});
        }

        // copy back to vector (these are pointers anyway, so copying is cheap)
        typename ArbitratorBase::Options sortedOptionsVector;
        sortedOptionsVector.reserve(options.size());
        for (const auto& sortedOption : sortedOptionsMap) {
            sortedOptionsVector.push_back(sortedOption.second);
        }
        return sortedOptionsVector;
    }
};
} // namespace behavior_planning
