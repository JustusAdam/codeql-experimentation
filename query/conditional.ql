import cpp
import FlowConfigs
import semmle.code.cpp.controlflow.Guards

module Flow = TaintTracking::Global<SourceToConditional>;

predicate controlsExec(Expr input, Expr dependent) {
  exists(GuardCondition guard, BasicBlock controlledBlock |
    input = guard.getAChild*() and
    guard.controls(controlledBlock, _) and
    dependent.getBasicBlock() = controlledBlock
  )
}

from DataFlow::Node source, DataFlow::Node target
where controlsExec(source.asExpr(), target.asExpr())
select source, target
