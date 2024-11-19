import Basics
import cpp
import semmle.code.cpp.dataflow.new.TaintTracking

// from DataFlow::Node n, DataFlow::Node store //, Type sens
// where
//   //   is_sensitive(sens) and
//   //   n.getType().refersTo(sens) and
//   is_store(store) and
//   StoreTaint::flow(n, store)
// select n, store, n.getType(), store.getLocation()
//
// from Call c, DataFlow::Node node
// where node.asExpr() = c.getArgument(1) and is_store_function(c.getTarget())
// select c, node, node.getLocation()
//
// from Call c, Expr argument, DataFlow::Node n
// where
//   is_store_function(c.getTarget()) and
//   argument = c.getArgument(1) and
//   argument = n.asIndirectArgument()
// select c, argument, n, c.getLocation()
//
// from Call c, Function f, Type t
// where
//   c.getTarget() = f and
//   f.getName() = "replace" and
//   f.getDeclaringType() = t
// select c, t, c.getLocation()
//
// from VariableAccess a, Variable v, Expr parent
// where
//   a.getEnclosingFunction().getName() = "questions_submit" and
//   v.getName() = "bg" and
//   parent.getAChild*() = a and
//   a.getTarget() = v
// select a, parent, a.getLocation()
//
from Expr e
where e.getEnclosingFunction().getName() = "questions_submit"
select e, e.getLocation().getStartLine()
