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
- [ ] Add argument with expected execution time in getCommand, gainControl, ...?
- [ ] add cost estimator / expected_cost() in the behavior class/object (in order to use its data/computations), similar to std::sort with compare function as class member or static function
- [ ] Cache computation results (in order not to recompute conditions for to_str(), but also when environment does not change in relevant fields)
- [ ] throw exception if gainControl is called with false invocationCondition
- [ ] write documentation for all basic classes
- [ ] think about proper handling of addOption for each arbitrator/coordinator when in control -> only problem for coordinators?
      (either disallow by exception or rather properly handle gain/loseControl of subbehaviors)


### nice to have
- [x] use `std::optional`
- [ ] Add an abstract / empty environment model to base library (also as constructor argument to behaviors)
      Maybe sth. like BehaviorWithContext
- [ ] pass EnvironmentModel only as ConstPtr to behaviors → read-only access (to prevent "hidden communication" / coupling between behaviors)
- [ ] enable cast operators for SubCommandT->CommandT conversion
      See https://devdocs.io/cpp/language/cast_operator
- [ ] Use static_assert to ensure our assumptions on the template types are met (see "Named requirements", "type_traits" and constexpr if)
- [ ] Use curiously recurring template pattern for static polymorphism?
      Could allow to guarantee correct behavior of specializations (e.g. invocationCondition should be tighter / include the one from base class), because the user is not overloading the whole e.g. checkInvocationCondition
      See also https://meetingcpp.com/mcpp/slides/2019/Modern%20Template%20Techniques.pdf
           and https://www.fluentcpp.com/posts/#templates
- [ ] (optional) parallelize sub-behaviors of arbitrators / coordinators
                 See also https://isocpp.org/wiki/faq/cpp11-library-concurrency
- [ ] (optional) enable optional sub-behaviors in ConjunctiveCoordinator
- [ ] (optional) arbitator with dynamically generated options (e.g. create new options in findBestOption if none of the given options is applicable or good enough)
- [ ] (optional) use iterator instead of index for activeBehavior_
      `behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();` would look like
      `activeBehavior_->behavior->checkCommitmentCondition();`

### low prio
- [ ] (lowPrio) use explicit constructors / cast operators for SubCommandT->CommandT conversion
                See https://google.github.io/styleguide/cppguide.html#Implicit_Conversions
- [ ] (lowPrio) ConjunctiveCoordinator: add flag loseControlWithoutInvocation
                (commitmentCondition would need one committed sub-behaviors AND true invocationCondition of all sub-behaviors)
- [x] (lowPrio) printout: add to_stream() to Option class
- [ ] (lowPrio) printout: format priority index to fixed number of digits (use fmt) `int n_max_digits = (int) log10 ((double) priority_arbitrator.behaviorOptions_.size()) + 1;`
- [ ] (lowPrio) printout: use fmt


## Open questions
- [ ] Should it be allowed to call getCommand without gainControl first (e.g. to estimate costs)?
      Maybe add `bool simulate` argument, to make allow it only by explicit calls (which will then also enable approximation for simulations)
      Beware of what it means for / how it is correctly dealt by arbitrators! (that should not call lose-&gainControl of sub-behaviors in simulation)

## Ideas from HFSM2 library (Hierarchical Finite State Machine)
- [ ] Rename gainControl() / loseControl() to enter() / exit()
- [ ] Get rid of getCommand() and its return value? (return value and passing the command through the tree, makes problems when command "types" differ among options)
      Use sth. like `void run()` / `void update()`, that publishes the command itself, instead.
      Would be more similar to FSMs, but also more flexible and make asynchronuous design easier.
- [ ] Add TimedBehavior which saves the entry time and provides `duration()`


## Arbitrator types
- Sequence
  - could be resumable
- ExpectedUtility / RankedWeightedRandom
  Kevin Dill - Dual-Utility Reasoning (Game AI Pro 2, 2015) - http://www.gameaipro.com/GameAIPro2/GameAIPro2_Chapter03_Dual-Utility_Reasoning.pdf
  https://en.wikipedia.org/wiki/Expected_utility_hypothesis
  https://github.com/andrew-gresyk/HFSM2/wiki/Utility-Theory
