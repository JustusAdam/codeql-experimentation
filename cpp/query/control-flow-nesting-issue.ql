import cpp
import semmle.code.cpp.controlflow.IRGuards

from Variable v, VariableAccess va, GuardCondition cond, Call c, int condition_line, int call_line
where
  c.getTarget().getName() = "call" and
  va.getTarget() = v and
  v.getName() = "condition" and
  cond.getAChild*() = va and
  cond.controls(c.getBasicBlock(), _) and
  condition_line = va.getLocation().getStartLine() and
  call_line = c.getLocation().getStartLine()
select v, va, cond, c, condition_line, call_line
