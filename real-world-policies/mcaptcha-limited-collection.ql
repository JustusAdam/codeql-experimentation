import cpp
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.controlflow.Guards

predicate controlsExec(Expr input, Expr dependent) {
  exists(GuardCondition guard, BasicBlock controlledBlock |
    input = guard.getAChild*() and
    guard.controls(controlledBlock, _) and
    dependent.getBasicBlock() = controlledBlock
  )
}

predicate is_store(Expr e) {
  exists(Call c, Function f |
    c.getTarget() = f and f.getName() = "analysis_save" and c.getArgument(0) = e
  )
}

predicate is_site_key(Expr e) {
  exists(FieldAccess fa, Field f |
    fa.getTarget() = f and
    f.getName() = "key" and
    f.getDeclaringType().getName() = "ApiWork" and
    e = fa
  )
}

predicate is_auth_check_call(Call c) { c.getTarget().getName() = "analytics_captcha_is_published" }

module FlowConfig implements DataFlow::ConfigSig {
  predicate isSink(DataFlow::Node n) { any() }

  predicate isSource(DataFlow::Node n) { any() }
}

module Flow = TaintTracking::Global<FlowConfig>;

// All object of interest
// from Expr e, string message
// where
//   is_store(e) and message = "store"
//   or
//   is_auth_check_call(e.(Call)) and message = "auth_check"
//   or
//   is_site_key(e) and message = "site_key"
// select e, message, e.getLocation().getStartLine()
//
// Complete query. Formulated positively, shows expression if there is no violation
from
  Expr store, Expr site_key, DataFlow::Node store_node, DataFlow::Node site_key_node,
  Call auth_check
where
  store_node.asExpr() = store and
  site_key_node.asExpr() = site_key and
  controlsExec(auth_check, store) and
  is_store(store) and
  is_site_key(site_key) and
  is_auth_check_call(auth_check) and
  Flow::flow(site_key_node, store_node)
select store, store.getLocation().getStartLine(), site_key, site_key.getLocation().getStartLine(),
  auth_check, auth_check.getLocation().getStartLine()
//
// Just the flow to store
// from
//   Expr store, Expr site_key, DataFlow::Node store_node, DataFlow::Node site_key_node,
//   Location store_loc, Location site_key_loc
// where
//   store_node.asExpr() = store and
//   site_key_node.asExpr() = site_key and
//   store.getLocation() = store_loc and
//   site_key.getLocation() = site_key_loc and
//   site_key_loc.getFile().getBaseName() = "main.cpp" and
//   store_loc.getFile().getBaseName() = "main.cpp" and
//   //is_store(store) and
//   is_site_key(site_key) and
//   Flow::flow(site_key_node, store_node)
// select site_key, site_key_loc.getStartLine(), store, store_loc.getStartLine()
//
// Control influence from auth check
// from
//   Expr store, DataFlow::Node store_node, Call auth_check, Location auth_check_loc,
//   Location store_loc
// where
//   store_node.asExpr() = store and
//   controlsExec(auth_check, store) and
//   is_store(store) and
//   is_auth_check_call(auth_check) and
//   auth_check.getLocation() = auth_check_loc and
//   store.getLocation() = store_loc
// select auth_check, auth_check_loc.getStartLine(), store, store_loc.getStartLine()
