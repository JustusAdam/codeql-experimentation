// Scope:
// Everywhere
// Definitions:
// 1. "stored_resource" is each "resource" marked resource where:
//     A. There is a "commit" marked commit where:
//         a. "commit" goes to "resource"
//     and
//     B. There is a "store" marked sink where:
//         a. "resource" goes to "store"
// 2. "set_new_resource_properties" is each "set_permissions" marked new_resource where:
//     A. There is a "stored_resource" where:
//         a. For each "get_parent_permissions" marked parent_property:
//             i) "stored_resource" does not go to "get_parent_permissions"
//         and
//         b. "stored_resource" goes to "set_permissions"
// Policy:
// 1. For each "set_new_resource_properties":
//     A. For each "stored_resource":
//         a. If "stored_resource" goes to "set_new_resource_properties" then:
//             i) There is a "auth_check" marked check_rights where:
//                 A) "stored_resource" goes to "auth_check"
//                 and
//                 B) "auth_check" affects whether "set_new_resource_properties" happens
import cpp
import semmle.code.cpp.dataflow.new.TaintTracking

predicate is_resource(Expr e) {
  //   exists(FunctionCall c |
  //     c.getQualifier() = e and
  //     c.getTarget().getName() = "set_propval"
  // )
  exists(Call c | c.getTarget().getName() = "set_propval" and c.getQualifier() = e)
}

predicate is_commit(Expr e) {
  (e instanceof ThisExpr or e instanceof ImplicitThisFieldAccess) and
  e.getEnclosingFunction().getName() = "apply_opts"
}

predicate is_check_rights(Expr e) { e.(FunctionCall).getTarget().getName() = "check_write" }

predicate is_sink(Expr e) {
  exists(FunctionCall c | c.getArgument(0) = e and c.getTarget().getName() = "add_resource")
}

// Sanity: highlights marked objects
//
// from Expr e, string message
// where
//   is_resource(e) and message = "stored resource"
//   or
//   is_commit(e) and message = "commit"
//   or
//   is_check_rights(e) and message = "check rights"
//   or
//   is_sink(e) and message = "sink"
// select e, message
module AllFlowsConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) { any() }

  predicate isSink(DataFlow::Node n) { any() }
}

module Flows = TaintTracking::Global<AllFlowsConfig>;

//     A. There is a "commit" marked commit where:
//         a. "commit" goes to "resource"
//
from DataFlow::Node commit, DataFlow::Node resource
where
  is_commit(commit.asExpr()) and
  is_resource(resource.asExpr()) and
  Flows::flow(commit, resource) and
  commit.getLocation().getFile().getBaseName() = "main.cpp"
select commit, resource, resource.getLocation()
//
//     B. There is a "store" marked sink where:
//         a. "resource" goes to "store"
//
// from DataFlow::Node store, DataFlow::Node resource
// where
//   is_sink(store.asExpr()) and
//   is_resource(resource.asExpr()) and
//   Flows::flow(resource, store)
// select resource, store, resource.getLocation()
//
// 1. "stored_resource" is each "resource" marked resource where:
//     A. There is a "commit" marked commit where:
//         a. "commit" goes to "resource"
//     and
//     B. There is a "store" marked sink where:
//         a. "resource" goes to "store"
// from DataFlow::Node commit, DataFlow::Node resource, DataFlow::Node store
// where
//   is_commit(commit.asExpr()) and
//   is_resource(resource.asExpr()) and
//   Flows::flow(commit, resource) and
//   is_sink(store.asExpr()) and
//   Flows::flow(resource, store) and
//   commit.getLocation().getFile().getBaseName() = "main.cpp"
// select commit, resource, store
//
// Relevant expressions
//
// from Expr e, DataFlow::Node n, string message
// where
//   (
//     is_sink(e) and message = "sink"
//     or
//     is_resource(e) and message = "resource"
//     or
//     is_commit(e) and message = "commit"
//     or
//     is_check_rights(e) and message = "check rights"
//   ) and
//   n.asExpr() = e
// select n, message
