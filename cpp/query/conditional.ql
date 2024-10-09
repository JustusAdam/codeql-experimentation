import cpp
import FlowConfigs
import semmle.code.cpp.controlflow.Guards

module Flow = TaintTracking::Global<SourceToConditional>;

predicate isInputToConditionalDeterminingExecution(DataFlow::Node input, DataFlow::Node dependent) {
  exists(GuardCondition guard, BasicBlock controlledBlock |
    // The input node is used in the guard condition
    input.asExpr() = guard.getAChild*() and
    // The guard controls the basic block containing the dependent node
    guard.controls(controlledBlock, _) and
    // The dependent node is in the controlled IRBlock
    dependent.asExpr().getBasicBlock() = controlledBlock
  )
}

from DataFlow::Node source, DataFlow::Node target
where isInputToConditionalDeterminingExecution(source, target)
select source, target
