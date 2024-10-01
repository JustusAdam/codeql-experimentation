// Scope:
// In delete
// Policy:
// 1. For each "stored_data" type marked user_data:
//     A. There is a "retrieval" that produces "stored_data" where:
//        a. There is a "deletes" marked to_delete where:
// 	       i) "retrieval" goes to "deletes"
import cpp
import semmle.code.cpp.dataflow.new.DataFlow
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.controlflow.ControlFlowGraph

predicate is_user_data(Type t) {
  t.getName() = "Comment" and
  t.getName() = "User" and
  t.getName() = "Post"
}

predicate is_delete(DataFlow::Node n) { n.asExpr().(FunctionCall).getTarget().getName() = "delete" }

module SourceSinkCallConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { is_user_data(source.asExpr().getType()) }

  predicate isSink(DataFlow::Node sink) { is_delete(sink) }
}

predicate is_root(Function n) { n.getName() = "delete_controller" }

predicate reachable_from_root(ControlFlowNode n) {
  reachable(n) and
  (
    is_root(n.getControlFlowScope())
    or
    exists(ControlFlowNode caller |
      // TODO this should probably also try and resolve dynamic dispatch
      caller.(FunctionCall).getTarget() = n.getControlFlowScope() and
      reachable_from_root(caller)
    )
  )
}

module Taint = TaintTracking::Global<SourceSinkCallConfig>;

from Type stored_data, DataFlow::Node retrieval, DataFlow::Node delete
where
  retrieval.asExpr().getType() = stored_data and
  is_user_data(stored_data) and
  (not Taint::flow(retrieval, delete) or not reachable_from_root(delete.asExpr()))
select stored_data, retrieval, delete
