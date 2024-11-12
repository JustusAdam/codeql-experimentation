// Scope:
// Everywhere
// Definitions:
// 1. "pageview_store" is each "store" marked db_store where:
// 	A. There is a "pageview" marked pageviews:
// 		a. "pageviews" goes to "store"
// # version where db_store is on argument 0, where the pageview goes
// Policy:
// 1. For each "pageview_store":
// 	A. There is a "date" marked "time" where:
// 		a. "date" goes to "pageview_store"'s operation
import cpp
import semmle.code.cpp.dataflow.new.DataFlow
import semmle.code.cpp.dataflow.new.TaintTracking
import Common

predicate is_store(Expr e) {
  exists(FunctionCall call |
    call.getArgument(0) = e and call.getTarget().getName() = "update_and_fetch"
  )
}

// Sanity, all relevant objects
// from Expr e, DataFlow::Node node, string message
// where
//   node.asExpr() = e and
//   (
//     is_pageview(e) and
//     message = "pageview"
//     or
//     is_store(e) and message = "store"
//     or
//     is_time(e) and message = "time"
//   )
// select e, node, message
module FlowConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) {
    //is_pageview(n.asExpr()) or is_time(n.asExpr())
    any()
  }

  predicate isSink(DataFlow::Node n) {
    //is_store(n.asExpr())
    any()
  }
}

module Flow = TaintTracking::Global<FlowConfig>;

from DataFlow::Node pageview, DataFlow::Node store //, DataFlow::Node time, DataFlow::Node op
where
  is_pageview(pageview.asExpr()) and
  is_store(store.asExpr()) and
  //is_time(time.asExpr()) and
  Flow::flow(pageview, store)
//   Flow::flow(time, op) and
//   exists(FunctionCall c | c.getAnArgument() = op.asExpr() and c.getAnArgument() = store.asExpr())
select pageview, store //, time
