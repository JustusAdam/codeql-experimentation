import cpp
import FlowConfigs

module AllFlows = PrintTaint;

// For some reason in the `vector` example with `for` loops this works,
// but using `SourceSinkCallTaint` does not
from DataFlow::Node source, DataFlow::Node sink
where
  AllFlows::flow(source, sink) and
  isSourceCall(source.asExpr()) and
  isTargetOperand(sink.asExpr()) //and source != sink
select source, sink
