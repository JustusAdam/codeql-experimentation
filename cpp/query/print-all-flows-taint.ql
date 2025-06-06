/**
 * Prints all taint flows in the program.
 */

import cpp
import FlowConfigs

from DataFlow::Node source, DataFlow::Node sink
where PrintTaint::flow(source, sink) and source != sink
select source, source.asExpr().getAPrimaryQlClass(), sink, sink.asExpr().getAPrimaryQlClass()
