// Scope:
// Somewhere
// Definitions:
// 1. "sensitive_types_that_are_stored" is each "sensitive" type marked sensitive where, anywhere in the application:
// 	A. There is a "source" that produces "sensitive" where:
// 		a. There is a "store" marked stores where:
// 			i) "source" goes to "store"
// Policy:
// 1. For each "sensitive_types_that_are_stored":
// 	A. There is a "source" that produces "sensitive_types_that_are_stored" where:
// 		a. There is a "deleter" marked deletes where:
// 			i) "source" goes to "deleter"
import cpp
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.pointsto.PointsTo
import Basics

predicate is_delete(Expr e) {
  exists(FunctionCall c | c.getTarget().getName() = "delete_" and c.getArgument(2) = e)
}

// Use of any() here is because we've observed that sometimes flows involving
// vectors are not captured correctly if isSource or isSink is meaningfully
// implemented.
module DeleteTaintConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) {
    any()
    //exists(Type t | is_sensitive(t) and n.getType().refersTo(t))
  }

  predicate isSink(DataFlow::Node n) {
    //is_delete(n.asExpr())
    // exists(DataFlow::Node i |
    //   i.asIndirectExpr(1) = n.asExpr() and
    //   is_delete(i.asExpr())
    // )
    any()
  }
}

module DeleteTaint = TaintTracking::Global<DeleteTaintConfig>;

// from Expr e, string message
// where
//   is_store(e) and message = "is store"
//   or
//   is_delete(e) and message = "is delete"
//   or
//   is_sensitive(e.getType()) and message = "is of sensitive type"
// select e, e.getType(), message
//
// from DataFlow::Node src, DataFlow::Node sink
// where DeleteTaint::flow(src, sink) and src.getLocation().getFile().getBaseName() = "questions.cpp"
// select src, sink
//
module FlowNodesFromDeleteConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) { n.getEnclosingCallable().getName() = "forget_user" }

  predicate isSink(DataFlow::Node n) { any() }
}

module FlowNodesFromDelete = TaintTracking::Global<FlowNodesFromDeleteConfig>;

from Expr delete_arg, Expr target
where
  // DeleteTaint::flow(data, target) and
  // is_sensitive(data.getType()) and
  is_delete(delete_arg) and
  pointer(delete_arg, target)
select delete_arg, target
//
// Testing that taint does not cross the `Value` constructor
//
// from DataFlow::Node num, DataFlow::Node vnum, Variable vnum_v, Function questions_submit // Parameter num_v
// where
//   // num_v.getName() = "num" and
//   vnum_v.getName() = "vnum" and
//   // num_v.getFunction() = questions_submit and
//   vnum_v.getEnclosingElement().(Function) = questions_submit and
//   questions_submit.getName() = "questions_submit" and
//   // num.asExpr().(VariableAccess).getTarget() = num_v and
//   vnum.asExpr().(VariableAccess).getTarget() = vnum_v and
//   AllFlows::flow(num, vnum)
// select num, vnum, num.getLocation().getStartLine()
