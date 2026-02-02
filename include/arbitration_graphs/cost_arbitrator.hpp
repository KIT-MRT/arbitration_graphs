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
    using PerOptionToBatchAdapterT = PerOptionToBatchAdapter<EnvironmentModelT, SubCommandT>;
    using PlaceboVerifierT = verification::PlaceboVerifier<EnvironmentModelT, SubCommandT>;
    using VerifierT = verification::Verifier<EnvironmentModelT, SubCommandT>;

    class Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NoFlags = 0b0, Interruptable = 0b1, Fallback = 0b10 };

        Option(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : ArbitratorBase::Option(behavior, flags) {
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
        mutable util_caching::Cache<Time, double> lastEstimatedCost_;
    };


    explicit CostArbitrator(const typename BatchCostEstimatorT::Ptr& batchCostEstimator,
                            const std::string& name = "CostArbitrator",
                            typename VerifierT::Ptr verifier = std::make_shared<PlaceboVerifierT>())
            : ArbitratorBase(name, verifier), costEstimator_{batchCostEstimator} {};

    explicit CostArbitrator(const typename CostEstimatorT::Ptr& costEstimator,
                            const std::string& name = "CostArbitrator",
                            typename VerifierT::Ptr verifier = std::make_shared<PlaceboVerifierT>())
            : ArbitratorBase(name, verifier),
              costEstimator_{std::make_shared<PerOptionToBatchAdapterT>(costEstimator)} {};


    void addOption(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior,
                   const typename Option::FlagsT& flags) override {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->addOptionImpl(option);
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

        std::vector<typename Option::Ptr> validOptions;
        for (auto& optionBase : options) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(optionBase);

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

        // prepare candidates for cost estimation
        std::vector<CandidateT> candidates;
        candidates.reserve(validOptions.size());
        for (const auto& option : validOptions) {
            const bool isActive = this->isActive(option);
            const std::optional<SubCommandT> command = option->getCommand(time, environmentModel);
            if (!command) {
                throw InvalidStateError("Could not retrieve cached command.");
            }
            candidates.push_back(CandidateT{command.value(), isActive});
        }

        std::vector<double> costs = costEstimator_->estimateCosts(time, environmentModel, candidates);
        if (costs.size() != candidates.size()) {
            throw InvalidCostError("CostEstimator returned mismatching number of costs.");
        }

        std::multimap<double, typename ArbitratorBase::Option::Ptr> sortedOptionsMap;
        for (std::size_t i = 0; i < validOptions.size(); ++i) {
            validOptions[i]->cacheLastEstimatedCost(time, costs[i]);
            sortedOptionsMap.insert({costs[i], validOptions[i]});
        }

        // copy back to vector (these are pointers anyway, so copying is cheap)
        typename ArbitratorBase::Options sortedOptionsVector;
        sortedOptionsVector.reserve(options.size());
        for (const auto& sortedOption : sortedOptionsMap) {
            sortedOptionsVector.push_back(sortedOption.second);
        }
        return sortedOptionsVector;
    }

    typename BatchCostEstimatorT::Ptr costEstimator_;
};
} // namespace arbitration_graphs

#include "internal/cost_arbitrator_io.hpp" // IWYU pragma: keep
