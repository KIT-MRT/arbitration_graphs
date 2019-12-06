# BEHAVIOR PLANNING

behavior planning based on simple atomic behaviors that are combined to more complex behaviors using arbitrators

### [Doxygen documentation](http://burger.private.MRT.pages.mrt.uni-karlsruhe.de/behavior_planning/doxygen/index.html)
### [Coverage report](http://burger.private.MRT.pages.mrt.uni-karlsruhe.de/behavior_planning/coverage/index.html)

## ToDos

- [x] template Command type
- [x] printout: add intendation by replacing "\n"
- [x] write base class for arbitrators
- [x] use enum for behavior options flags (to combine them with "|" while keeping the signature)
- [x] generalize prio and cost arbitrators to use SubCommandT -> add test to enable SubCommandT != CommandT by providing CommandT(const SubCommandT&) constructor
- [x] implement ConjunctiveCoordinator
- [ ] throw exception if gainControl is called with false invocationCondition
- [ ] write documentation for all basic classes
- [ ] think about proper handling of addOption for each arbitrator/coordinator when in control -> only problem for coordinators?
      (either disallow by exception or rather properly handle gain/loseControl of subbehaviors)

### nice to have
- [x] use `std::optional`
- [ ] Use static_assert to ensure our assumptions on the template types are met
- [ ] (optional) add option to have a cost estimator in the behavior class/object (in order to use its data/computations), similar to std::sort with compare function as class member or static function
- [ ] (optional) arbitator with dynamically generated options (e.g. create new options in findBestOption if none of the given options is applicable or good enough)
- [ ] (optional) use iterator instead of index for activeBehavior_  
      `behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();` would look like  
      `activeBehavior_->behavior->checkCommitmentCondition();`

### low prio
- [ ] (lowPrio) ConjunctiveCoordinator: add flag loseControlWithoutInvocation
                (commitmentCondition would need one committed sub-behaviors AND true invocationCondition of all sub-behaviors)
- [x] (lowPrio) printout: add to_stream() to Option class
- [ ] (lowPrio) printout: format priority index to fixed number of digits (use fmt) `int n_max_digits = (int) log10 ((double) priority_arbitrator.behaviorOptions_.size()) + 1;`
- [ ] (lowPrio) printout: use fmt


## Open questions
- [ ] Should we incorporate expected execution time in getCommand, gainControl, ...?
- [ ] Add an abstract / empty environment model to base library (also as constructor argument to behaviors)?
