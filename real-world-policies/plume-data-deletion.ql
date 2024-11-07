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
import semmle.code.cpp.ir.IR

predicate is_user_data(Type outer) {
  exists(Type t |
    is_user_data_direct(t) and
    outer.refersTo(t)
  )
}

predicate is_user_data_direct(Type t) {
  t.getName() = "Comment" or
  t.getName() = "User" or
  t.getName() = "Post" or
  t.getName() = "Blog"
}

// It is fine to use `Parameter` here, because the source of the function is in
// scope and so we see the parameter.
predicate is_delete(DataFlow::Node n) {
  n.asParameter().getFunction().getName().regexpMatch(".*deleteAny.*") and
  n.asParameter().getIndex() = 0
}

module SourceSinkCallConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { is_user_data(source.getType()) }

  predicate isSink(DataFlow::Node sink) { is_delete(sink) }
}

predicate is_root(Function n) { n.getName() = "deleteUserController" }

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

// from Type stored_data
// where
//   is_user_data(stored_data) and
//   not exists(DataFlow::Node retrieval, DataFlow::Node delete |
//     retrieval.asExpr().(FunctionCall).getExpectedReturnType().refersTo(stored_data) and
//     Taint::flow(retrieval, delete) //or not reachable_from_root(delete.asExpr()))
//   )
// select stored_data
from Type user_data
where
  is_user_data_direct(user_data) and
  not exists(DataFlow::Node source, DataFlow::Node sink |
    source.getEnclosingCallable().getName() = "deleteUserController" and
    source.getType().refersTo(user_data) and
    Taint::flow(source, sink)
  )
//where delete.asExpr().(FunctionCall).getTarget().getName()
select user_data

module AllConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { any() }

  predicate isSink(DataFlow::Node sink) { any() }
}

module AllTaint = TaintTracking::Global<AllConfig>;
// from DataFlow::Node src, DataFlow::Node snk, Function f, Type t
// where
//   is_delete(snk) and
//   is_user_data_direct(t) and
//   src.getType().refersTo(t) and
//   AllTaint::flow(src, snk) and
//   f = snk.asParameter().getFunction() and
//   src.getEnclosingCallable().getName() = "deleteUserController"
// select t, src, snk
