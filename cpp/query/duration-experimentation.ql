import cpp
import semmle.code.cpp.dataflow.new.TaintTracking

module AllFlowsConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { any() }

  predicate isSink(DataFlow::Node sink) { any() }

  predicate isAdditionalFlowStep(DataFlow::Node n1, DataFlow::Node n2) {
    exists(string s |
      (s = "__d" or s = "__r") and
      (
        exists(FieldAccess f |
          f.getTarget().getName() = s and
          n1.asIndirectExpr() = f.getQualifier() and
          n2.asExpr() = f
        )
        or
        exists(ImplicitThisFieldAccess f |
          f.getTarget().getName() = s and
          n1.asIndirectExpr() = f.getQualifier() and
          n2.asExpr() = f
        )
      )
    )
  }
}

module Flow = TaintTracking::Global<AllFlowsConfig>;

// from VariableAccess v, string s
// where
//   v.getTarget().getName() = s and
//   (s = "duration" or s = "count")
// select v
//
from DataFlow::Node n1, DataFlow::Node n2, VariableAccess now, Call c
where
  Flow::flow(n1, n2) and
  n1.asExpr() = c and
  n2.asExpr() = now and
  now.getTarget().getName() = "now" and
  c.getTarget().getName() = "now"
select n1, n2
//
// from DataFlow::Node n1, DataFlow::Node n2, Call c
// where
//   Flow::flow(n1, n2) and
//   n1.asExpr() = c and
//   c.getTarget().getName() = "now"
// select n1, n2, n2.getLocation()
//
// from Expr e, Expr qual, string field_name
// where
//   (
//     exists(FieldAccess f |
//       f.getTarget().getName() = field_name and
//       e = f and
//       f.getQualifier() = qual
//     )
//     or
//     exists(ImplicitThisFieldAccess f |
//       f.getTarget().getName() = field_name and
//       e = f and
//       f.getQualifier() = qual
//     )
//   ) and
//   field_name = "__d"
// select e, qual, e.getLocation()
