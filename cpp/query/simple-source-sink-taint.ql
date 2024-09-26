/**
 * @id paralegal/comptest
 * @kind problem
 * @severity warning
 */

import cpp
import FlowConfigs

from DataFlow::Node source, DataFlow::Node sink
where SourceSinkCallTaint::flow(source, sink) //and source != sink
select source, source.asExpr().getAPrimaryQlClass(), sink, sink.asExpr().getAPrimaryQlClass()
