/**
 * @id paralegal/comptest
 * @kind problem
 * @severity warning
 */

import cpp
import semmle.code.cpp.dataflow.new.DataFlow

module MyFlowConfiguration implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { any() }

  predicate isSink(DataFlow::Node sink) { any() }
}

module MyFlow = DataFlow::Global<MyFlowConfiguration>;

from DataFlow::Node source, DataFlow::Node sink
where MyFlow::flow(source, sink) and source != sink
select source, source.asExpr().getAPrimaryQlClass(), sink, sink.asExpr().getAPrimaryQlClass()
