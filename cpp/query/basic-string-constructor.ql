import cpp
import semmle.code.cpp.dataflow.new.TaintTracking

module FlowConfig implements DataFlow::ConfigSig {
  predicate isSink(DataFlow::Node n) { any() }

  predicate isSource(DataFlow::Node n) { any() }

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

predicate is_sensitive(Type t) { t.getName() = "Identity" }

predicate is_delete(Expr e) {
  exists(Call c |
    c.getTarget().getName() = "delete_user" and
    c.getArgument(0) = e
  )
}

predicate is_source(Expr e) { e.(Call).getTarget().getName() = "identity" }

module Flow = TaintTracking::Global<FlowConfig>;

// full query
from
  //Type t,
  Expr retrieval, Expr delete, DataFlow::Node retrieval_node, DataFlow::Node delete_node
where
  //is_sensitive(t) and
  //retrieval.getType().refersTo(t) and
  is_source(retrieval) and
  is_delete(delete) and
  retrieval_node.asExpr() = retrieval and
  delete_node.asIndirectExpr() = delete and
  Flow::flow(retrieval_node, delete_node) and
  retrieval.getLocation().getFile().getBaseName() = "main.cpp"
select retrieval,
  //t,
  retrieval.getLocation().getStartLine(), delete, delete.getLocation().getStartLine()
