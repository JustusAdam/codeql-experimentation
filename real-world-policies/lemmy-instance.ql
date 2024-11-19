// Scope:
// Everywhere
//
// Policy:
// 1. For each "db_access" marked db_access:
//     A. If "db_access" is not marked db_user_read then:
//         a. There is a "dc" marked instance_delete_check where:
//             i) "dc" influences "db_access"
//         and
//         b. There is a "bc" marked instance_ban_check where:
//             i) "bc" influences "db_access"
import cpp
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.controlflow.Guards

predicate is_db_access(Expr e) {
  // marked `db_access`
  exists(string name |
    e.(FunctionCall).getTarget().getName() = name and
    (
      name = "readLocalSite" or
      name = "findUserByEmailOrName"
    )
  )
}

predicate is_user_read(Expr e) {
  // marked `db_user_read`
  e.(FunctionCall).getTarget().getName() = "findUserByEmailOrName"
}

predicate is_delete_check(Expr e) {
  // marked `instance_delete_check`
  isLocalUserViewPersonDeleted(e)
}

predicate isLocalUserViewPersonDeleted(Expr e) {
  exists(FieldAccess bannedAccess, FieldAccess personAccess, VariableAccess localUserViewAccess |
    e = bannedAccess and
    bannedAccess.getTarget().getName() = "deleted" and
    bannedAccess.getQualifier() = personAccess and
    personAccess.getTarget().getName() = "person" and
    personAccess.getQualifier() = localUserViewAccess and
    localUserViewAccess.getTarget().getName() = "local_user_view"
  )
}

predicate is_ban_check(Expr e) {
  // marked `instance_ban_check`
  isLocalUserViewPersonBanned(e)
}

predicate isLocalUserViewPersonBanned(Expr e) {
  exists(FieldAccess bannedAccess, FieldAccess personAccess, VariableAccess localUserViewAccess |
    e = bannedAccess and
    bannedAccess.getTarget().getName() = "banned" and
    bannedAccess.getQualifier() = personAccess and
    personAccess.getTarget().getName() = "person" and
    personAccess.getQualifier() = localUserViewAccess and
    localUserViewAccess.getTarget().getName() = "local_user_view"
  )
}

predicate controlsExec(Expr input, Expr dependent) {
  exists(GuardCondition guard, BasicBlock controlledBlock |
    input = guard.getAChild*() and
    guard.controls(controlledBlock, _) and
    dependent.getBasicBlock() = controlledBlock
  )
}

module FlowConfig implements DataFlow::ConfigSig {
  predicate isSink(DataFlow::Node n) {
    // This could also be just inputs to conditionals
    any()
  }

  predicate isSource(DataFlow::Node n) { is_delete_check(n.asExpr()) or is_ban_check(n.asExpr()) }
}

module Flow = TaintTracking::Global<FlowConfig>;

// Actual policy
// from Expr db_access, DataFlow::Node condition
// where
//   exists(DataFlow::Node dc_n, DataFlow::Node bc_n |
//     is_delete_check(dc_n.asExpr()) and
//     Flow::flow(dc_n, condition) and
//     is_ban_check(bc_n.asExpr()) and
//     Flow::flow(bc_n, condition)
//   ) and
//   controlsExec(condition.asExpr(), db_access)
// select db_access
//
// Sanity 1: Lists all important nodes with a message describing their purpose
// from Expr e, string message
// where
//   is_db_access(e) and message = "db_access"
//   or
//   is_user_read(e) and message = "user_read"
//   or
//   is_delete_check(e) and message = "delete_check"
//   or
//   is_ban_check(e) and message = "ban_check"
// select e, message
//
// Simplified policy. Apparently this is sufficient.
from Expr db_access
where
  is_db_access(db_access) and
  not is_user_read(db_access) and
  not exists(Expr check |
    (is_delete_check(check) or is_ban_check(check)) and
    controlsExec(check, db_access)
  )
select db_access, db_access.getLocation()
