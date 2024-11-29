import cpp
import semmle.code.cpp.dataflow.new.TaintTracking

module FlowConfig implements DataFlow::ConfigSig {
  predicate isSink(DataFlow::Node n) {
    any()
    //is_delete(n.asExpr()) or is_delete(n.asIndirectExpr())
  }

  predicate isSource(DataFlow::Node n) {
    any()
    // exists(Type t, Expr e |
    //   is_sensitive(t) and
    //   e.getType().refersTo(t) and
    //   (
    //     e = n.asExpr() or
    //     e = n.asIndirectExpr()
    //   )
    // )
  }

  predicate isAdditionalFlowStep(DataFlow::Node n1, DataFlow::Node n2) {
    exists(Call c, Function f |
      (
        c.getQualifier() = n1.asExpr() or
        c.getQualifier() = n1.asIndirectExpr()
      ) and
      n2.asExpr() = c and
      c.getTarget() = f and
      f.getName() = "identity" and
      f.getDeclaringType().getName() = "Identity"
    )
  }
}

module Flow = TaintTracking::Global<FlowConfig>;

predicate is_sensitive(Type t) { t.getName() = "Identity" }

predicate is_delete(Expr e) {
  exists(Call c |
    c.getTarget().getName() = "delete_user" and
    c.getArgument(0) = e
  )
}

// full query
from Type t, Expr retrieval, Expr delete, DataFlow::Node retrieval_node, DataFlow::Node delete_node
where
  is_sensitive(t) and
  is_delete(delete) and
  retrieval.getType().refersTo(t) and
  retrieval_node.asExpr() = retrieval and
  delete_node.asExpr() = delete and
  Flow::flow(retrieval_node, delete_node) and
  retrieval.getLocation().getFile().getBaseName() = "main.cpp"
select t, retrieval, retrieval.getLocation().getStartLine(), delete,
  delete.getLocation().getStartLine()
//
// from Type t
// where is_sensitive(t)
// select t
//
// Delete sinks
// from Expr e
// where is_delete(e)
// select e
//
// Retrievals
// from Expr e
// where
//   exists(Type t | is_sensitive(t) and e.getType().refersTo(t)) and
//   e.getLocation().getFile().getBaseName() = "main.cpp"
// select e
//
// Flow step occurrence
// from DataFlow::Node n1, DataFlow::Node n2
// where FlowConfig::isAdditionalFlowStep(n1, n2)
// select n1, n2
