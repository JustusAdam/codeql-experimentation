/**
 * Prints all data flows from a direct function call to `source` to a direct
 * function call to `target`.
 */

import cpp
import FlowConfigs

from DataFlow::Node source, DataFlow::Node sink
where SourceSinkCallDataFlow::flow(source, sink) //and source != sink
select source, source.asExpr().getAPrimaryQlClass(), sink, sink.asExpr().getAPrimaryQlClass()
