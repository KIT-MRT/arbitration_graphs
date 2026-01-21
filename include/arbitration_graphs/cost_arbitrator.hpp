#pragma once

#include <memory>
#include <optional>

#include <util_caching/cache.hpp>
#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"
#include "exceptions.hpp"
#include "types.hpp"


namespace arbitration_graphs {

template <typename EnvironmentModelT, typename SubCommandT>
struct CostEstimator {
    using Ptr = std::shared_ptr<CostEstimator>;
    using ConstPtr = std::shared_ptr<const CostEstimator>;

    virtual double estimateCost(const Time& time,
                                const EnvironmentModelT& environmentModel,
                                const SubCommandT& command,
                                bool isActive) = 0;
};

template <typename EnvironmentModelT, typename SubCommandT>
struct BatchCostEstimator {
    using Ptr = std::shared_ptr<BatchCostEstimator>;
    using ConstPtr = std::shared_ptr<const BatchCostEstimator>;

    struct Candidate {
        SubCommandT command;
        bool isActive;
    };

    virtual std::vector<double> estimateCosts(const Time& time,
                                              const EnvironmentModelT& environmentModel,
                                              const std::vector<Candidate>& candidates) = 0;
};

template <typename EnvironmentModelT, typename SubCommandT>
class PerOptionToBatchAdapter : public BatchCostEstimator<EnvironmentModelT, SubCommandT> {
public:
    using CandidateT = typename BatchCostEstimator<EnvironmentModelT, SubCommandT>::Candidate;
    using CostEstimatorT = CostEstimator<EnvironmentModelT, SubCommandT>;
    explicit PerOptionToBatchAdapter(typename CostEstimatorT::Ptr perOptionEstimator)
            : perOptionEstimator_(std::move(perOptionEstimator)) {
    }
    std::vector<double> estimateCosts(const Time& time,
                                      const EnvironmentModelT& environmentModel,
                                      const std::vector<CandidateT>& candidates) override {
        std::vector<double> costs;
        costs.reserve(candidates.size());
        for (const auto& candidate : candidates) {
            costs.push_back(
                perOptionEstimator_->estimateCost(time, environmentModel, candidate.command, candidate.isActive));
        }
        return costs;
    }

private:
    typename CostEstimatorT::Ptr perOptionEstimator_;
};

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT = CommandT>
class CostArbitrator : public Arbitrator<EnvironmentModelT, CommandT, SubCommandT> {
public:
    using ArbitratorBase = Arbitrator<EnvironmentModelT, CommandT, SubCommandT>;

    using Ptr = std::shared_ptr<CostArbitrator>;
    using ConstPtr = std::shared_ptr<const CostArbitrator>;

    using BatchCostEstimatorT = BatchCostEstimator<EnvironmentModelT, SubCommandT>;
    using CandidateT = typename BatchCostEstimatorT::Candidate;
    using CostEstimatorT = CostEstimator<EnvironmentModelT, SubCommandT>;
    using PlaceboVerifierT = verification::PlaceboVerifier<EnvironmentModelT, SubCommandT>;
    using VerifierT = verification::Verifier<EnvironmentModelT, SubCommandT>;

    class Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NoFlags = 0b0, Interruptable = 0b1, Fallback = 0b10 };

        Option(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior,
               const FlagsT& flags,
               const typename BatchCostEstimatorT::Ptr& costEstimator)
                : ArbitratorBase::Option(behavior, flags), costEstimator_{costEstimator} {
        }

        typename BatchCostEstimatorT::Ptr costEstimator() const {
            return costEstimator_;
        }

        std::optional<double> lastEstimatedCost(const Time& time) const {
            return lastEstimatedCost_.cached(time);
        }
        void cacheLastEstimatedCost(const Time& time, const double& cost) const {
            lastEstimatedCost_.cache(time, cost);
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output            Output stream to write into, will be returned also
         * \param time              Expected execution time point of this behaviors command
         * \param environmentModel  A read-only object containing the current state of the environment
         * \param optionIndex       Position index of this option within options()
         * \param prefix            A string that should be prepended to each line that is written to the output stream
         * \param suffix            A string that should be appended to each line that is written to the output stream
         * \return                  The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::toStream()
         */
        std::ostream& toStream(std::ostream& output,
                               const Time& time,
                               const EnvironmentModelT& environmentModel,
                               const int& optionIndex,
                               const std::string& prefix = "",
                               const std::string& suffix = "") const override;

        /*!
         * \brief Returns a yaml representation of this option with its current state
         *
         * \param time  Expected execution time point of this behaviors command
         * \return      Yaml representation of this behavior
         */
        YAML::Node toYaml(const Time& time, const EnvironmentModelT& environmentModel) const override;

    private:
        typename BatchCostEstimatorT::Ptr costEstimator_;
        mutable util_caching::Cache<Time, double> lastEstimatedCost_;
    };


    explicit CostArbitrator(const std::string& name = "CostArbitrator",
                            typename VerifierT::Ptr verifier = std::make_shared<PlaceboVerifierT>())
            : ArbitratorBase(name, verifier) {};

    void addOption(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior,
                   const typename Option::FlagsT& flags,
                   const typename BatchCostEstimatorT::Ptr& batchCostEstimator) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags, batchCostEstimator);
        this->addOptionImpl(option);
    }


    void addOption(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior,
                   const typename Option::FlagsT& flags,
                   const typename CostEstimatorT::Ptr& costEstimator) {
        typename BatchCostEstimatorT::Ptr batchEstimator =
            std::make_shared<PerOptionToBatchAdapter<EnvironmentModelT, SubCommandT>>(costEstimator);
        addOption(behavior, flags, batchEstimator);
    }

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \param environmentModel  A read-only object containing the current state of the environment
     * \return      Yaml representation of this behavior
     */
    YAML::Node toYaml(const Time& time, const EnvironmentModelT& environmentModel) const override;

private:
    /*!
     * Find behavior option with lowest cost and true invocation condition
     *
     * @return  Applicable option with lowest costs (can also be the currently active option)
     */
    typename ArbitratorBase::Options sortOptionsByGivenPolicy(
        const typename ArbitratorBase::Options& options,
        const Time& time,
        const EnvironmentModelT& environmentModel) const override {

        using CandidateT = typename BatchCostEstimatorT::Candidate;

        std::unordered_map<typename BatchCostEstimatorT::Ptr, std::vector<typename Option::Ptr>> optionsByEstimator;
        for (auto& optionBase : options) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(optionBase);
            optionsByEstimator[option->costEstimator()].push_back(option);
        }

        std::multimap<double, typename ArbitratorBase::Option::Ptr> sortedOptionsMap;

        for (const auto& group : optionsByEstimator) {
            auto estimator = group.first;
            auto& groupedOptions = group.second;

            std::vector<typename Option::Ptr> validOptions;
            for (auto& option : groupedOptions) {
                const bool isActive = this->isActive(option);

                std::optional<SubCommandT> command;
                if (isActive) {
                    command = this->getAndVerifyCommand(option, time, environmentModel);
                } else {
                    option->behavior()->gainControl(time, environmentModel);
                    command = this->getAndVerifyCommand(option, time, environmentModel);
                    option->behavior()->loseControl(time, environmentModel);
                }
                if (!command) {
                    continue;
                }

                validOptions.push_back(option);
            }

            if (validOptions.empty()) {
                continue;
            }

            std::vector<CandidateT> candidates;
            candidates.reserve(validOptions.size());
            for (const auto& option : validOptions) {
                const bool isActive = this->isActive(option);
                const SubCommandT command = option->getCommand(time, environmentModel);
                candidates.push_back(CandidateT{command, isActive});
            }

            const std::vector<double> estimatedCosts = estimator->estimateCosts(time, environmentModel, candidates);

            if (estimatedCosts.size() != validOptions.size()) {
                throw InvalidCostError("CostEstimator returned invalid number of costs!");
            }

            for (std::size_t i = 0; i < validOptions.size(); i++) {
                const double cost = estimatedCosts[i];
                validOptions[i]->cacheLastEstimatedCost(time, cost);
                sortedOptionsMap.insert({cost, validOptions[i]});
            }
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
} // namespace arbitration_graphs

#include "internal/cost_arbitrator_io.hpp" // IWYU pragma: keep
