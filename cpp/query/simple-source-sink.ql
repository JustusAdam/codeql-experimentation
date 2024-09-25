/**
 * @id paralegal/comptest
 * @kind problem
 * @severity warning
 */

import cpp
import semmle.code.cpp.dataflow.new.DataFlow

module MyFlowConfiguration implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    source.asExpr().(FunctionCall).getTarget().getName() = "source"
  }

  predicate isSink(DataFlow::Node sink) {
    sink.asExpr().(FunctionCall).getTarget().getName() = "target"
  }
}

module MyFlow = DataFlow::Global<MyFlowConfiguration>;

from DataFlow::Node source, DataFlow::Node sink
where MyFlow::flow(source, sink) //and source != sink
select source, source.asExpr().getAPrimaryQlClass(), sink, sink.asExpr().getAPrimaryQlClass()
