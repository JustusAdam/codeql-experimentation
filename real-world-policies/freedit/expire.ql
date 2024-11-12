// Scope:
// In user_chron_job
// Policy:
// 1. For each "pageview" marked pageviews:
// 	A. There is a "expiration_check" marked expiration_check where:
// 		a. There is a "current_time" marked time where:
// 			i) "current_time" goes to "expiration_check"
// 		and
// 		b. There is a "fetched_pageview_date" marked db_access where:
// 			i) "pageview" goes to "fetched_pageview_date"
// 			and
// 			ii) "fetched_pageview_date" goes to "expiration_check"
// 		and
// 		c. There is a "deleter" marked deletes where:
//             i) "pageview" goes to "deleter"
//             and
// 			ii) "expiration_check" affects whether "deleter" happens
import cpp
import semmle.code.cpp.dataflow.new.DataFlow
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.controlflow.Guards
import Common

predicate is_expiration_check(Expr e) {
  exists(string s, Call c |
    c.getTarget().getName() = s and
    c = e and
    (
      s = "operator<"
      or
      s = "operator=>"
    )
  )
  or
  e instanceof ComparisonOperation
}

predicate is_removed(Expr e) {
  exists(Call c |
    c.getTarget().getName() = "remove" and
    c.getAnArgument() = e
  )
}

module FlowConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) { any() }

  predicate isSink(DataFlow::Node n) { any() }
}

module Flow = TaintTracking::Global<FlowConfig>;

// Finding the expiration check control flow influence
//
// from Expr remove, Expr guard, GuardCondition c
// where
//   guard = c and
//   is_removed(remove) and
//   c.controls(remove.getBasicBlock(), _) and
//   is_expiration_check(guard)
// select guard, remove, c, guard.getLocation()
//
// Flow from time to expiration check
from Expr remove, Expr guard, GuardCondition c, DataFlow::Node time, DataFlow::Node guard_node
where
  Flow::flow(time, guard_node) and
  guard_node.asExpr() = guard.getAChild() and
  guard = c and
  is_removed(remove) and
  c.controls(remove.getBasicBlock(), _) and
  is_expiration_check(guard) and
  remove.getEnclosingFunction().getName() = "clear_invalid"
select guard, remove, time, time.getLocation()
