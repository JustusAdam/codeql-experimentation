/**
 * Contains the configuration for how we evaluate dataflow and taint tracking
 * in the simple examples.
 *
 * Codeql distinguishes between data flow and taint, though both analyses
 * are parameterized over the same module signature (`isSource`, `isSink`, etc).
 * Explanations as to the actual difference are found in
 * [the documentation](https://codeql.github.com/docs/codeql-language-guides/analyzing-data-flow-in-cpp/#using-global-taint-tracking)
 * but the gist is that if a dataflow query appears to exhibit a false negative,
 * you should try rerunning it but replace all calls to dataflow with calls to
 * taint tracking, using the same source/sink configuration. This should generally
 * work as both the DataFlow and TaintTracking modules have the same API.
 */

private import cpp
import semmle.code.cpp.dataflow.new.DataFlow
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.ir.IR

predicate isTargetFunction(Function f) { f.getName() = "target" }

predicate isTargetParameter(Parameter p) {
  isTargetFunction(p.getFunction()) and
  p.getIndex() = 0
}

predicate isTargetOperand(Expr o) {
  exists(FunctionCall call | call.getTarget().getName() = "target" and call.getArgument(0) = o)
}

predicate isSourceCall(Expr e) { e.(FunctionCall).getTarget().getName() = "source" }

module AllFlowsConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { any() }

  predicate isSink(DataFlow::Node sink) { any() }
}

module SourceSinkCallConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { isSourceCall(source.asExpr()) }

  predicate isSink(DataFlow::Node sink) {
    isTargetOperand(sink.asExpr())
    //isTargetFunction(sink.asExpr().(FunctionCall).getTarget())
    //isTargetParameter(sink.asParameter())
  }
}

module SourceToConditional implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { isSourceCall(source.asExpr()) }

  predicate isSink(DataFlow::Node sink) {
    sink.asOperand().getUse() instanceof ConditionalBranchInstruction
  }
}

module PrintDataFlow = DataFlow::Global<AllFlowsConfig>;

module PrintTaint = TaintTracking::Global<AllFlowsConfig>;

module SourceSinkCallDataFlow = DataFlow::Global<SourceSinkCallConfig>;

module SourceSinkCallTaint = TaintTracking::Global<SourceSinkCallConfig>;
