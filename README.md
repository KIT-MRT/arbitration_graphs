# BEHAVIOR PLANNING

behavior planning based on simple atomic behaviors that are combined to more complex behaviors using arbitrators

### [Doxygen documentation](http://burger.private.MRT.pages.mrt.uni-karlsruhe.de/behavior_planning/doxygen/index.html)
### [Coverage report](http://burger.private.MRT.pages.mrt.uni-karlsruhe.de/behavior_planning/coverage/index.html)

## ToDos

- [ ] (optional) use iterator instead of index for activeBehavior_  
      `behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();` would look like  
      `activeBehavior_->behavior->checkCommitmentCondition();`
- [x] template Command type
