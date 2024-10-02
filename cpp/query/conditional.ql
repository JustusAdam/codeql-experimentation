import cpp
import FlowConfigs

module Flow = TaintTracking::Global<SourceToConditional>;

from DataFlow::Node source, DataFlow::Node cond, FunctionCall call
where
  isTargetFunction(call.getTarget()) and
  Flow::flow(source, cond) and 
  call.getParent+() = cond.asOperand().getUse().
select source
