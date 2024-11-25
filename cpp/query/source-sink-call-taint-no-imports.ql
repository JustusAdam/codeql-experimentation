import cpp
import semmle.code.cpp.dataflow.new.TaintTracking

module SourceSinkCallConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    source.asExpr().(Call).getTarget().getName() = "source"
  }

  predicate isSink(DataFlow::Node sink) {
    exists(Call call |
      call.getTarget().getName() = "target" and
      call.getArgument(0) = sink.asExpr()
    )
  }
}

module SourceSinkCallTaint = TaintTracking::Global<SourceSinkCallConfig>;

from DataFlow::Node source, DataFlow::Node sink, int source_line, int sink_line
where
  SourceSinkCallTaint::flow(source, sink) and
  source_line = source.getLocation().getStartLine() and
  sink_line = sink.getLocation().getStartLine()
select source, source_line, sink, sink_line
//
// from DataFlow::Node n, string message
// where
//   SourceSinkCallConfig::isSource(n) and message = "source"
//   or
//   SourceSinkCallConfig::isSink(n) and message = "sink"
// select n, message
