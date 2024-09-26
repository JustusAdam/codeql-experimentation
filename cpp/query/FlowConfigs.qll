/**
 * Contains the configuration for how we evaluate dataflow and taint tracking
 * in the simple examples.
 */

private import cpp
import semmle.code.cpp.dataflow.new.DataFlow
import semmle.code.cpp.dataflow.new.TaintTracking

module AllFlowsConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { any() }

  predicate isSink(DataFlow::Node sink) { any() }
}

module SourceSinkCallConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    source.asExpr().(FunctionCall).getTarget().getName() = "source"
  }

  predicate isSink(DataFlow::Node sink) {
    sink.asExpr().(FunctionCall).getTarget().getName() = "target"
  }
}

module PrintDataFlow = DataFlow::Global<AllFlowsConfig>;

module PrintTaint = TaintTracking::Global<AllFlowsConfig>;

module SourceSinkCallDataFlow = DataFlow::Global<SourceSinkCallConfig>;

module SourceSinkCallTaint = TaintTracking::Global<SourceSinkCallConfig>;
