/**
 * Prints all taint flows from a direct function call to `source` to a direct
 * function call to `target`.
 */

import cpp
import FlowConfigs

module AllFlows = PrintTaint;

// Print what reaches the sink
// from DataFlow::Node src, DataFlow::Node target //, DataFlow::Node last
// where
//   //src.getLocation().getFile().getBaseName() = "main.cpp" and
//   isTargetOperand(target.asExpr()) and
//   AllFlows::flow(src, target)
// select src // last, last.getLocation()
//
// Print where sources go
// from DataFlow::Node src, DataFlow::Node target //, DataFlow::Node last
// where
//   src.getLocation().getFile().getBaseName() = "main.cpp" and
//   isSourceCall(src.asExpr()) and
//   AllFlows::flow(src, target)
// select target // last, last.getLocation()
//
// Version for vectors where sometimes the flow is only found if we query all
// flows instead of configured ones.
//
// from DataFlow::Node source, DataFlow::Node sink
// where
//   AllFlows::flow(source, sink) and
//   SourceSinkCallConfig::isSource(source) and
//   SourceSinkCallConfig::isSink(sink)
// select source, sink
//
from DataFlow::Node source, DataFlow::Node sink
where SourceSinkCallTaint::flow(source, sink)
select source, //source.asExpr().getAPrimaryQlClass(),
  sink //, sink.asExpr().getAPrimaryQlClass()
