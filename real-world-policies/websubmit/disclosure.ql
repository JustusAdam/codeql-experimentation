// Scope:
// Everywhere
// Definitions:
// 1. "blessed" is each "safe_with_bless" marked safe_source_with_bless where:
// 	A. There is a "blesser" marked bless_safe_source where:
// 		a. "blesser" influences "safe_with_bless"
// 2. "sensitive_sink" is each "sink" marked sink where:
// 	A. There is a "sensitive" marked sensitive where:
// 		a. "sensitive" goes to "sink"
// 3. "sensitive_scope" is each "scope" marked scopes where:
// 	A. There is a "sensitive_sink" where:
// 		a. "scope" goes to "sensitive_sink"'s operation
// Policy:
// 1. For each "sensitive_sink":
// 	A. There is a "scope" marked scopes where:
// 		a. "scope" goes to "sensitive_sink"'s operation
// and
// 2. Each "root" input goes to a "sensitive_scope" only via a "blessed" or
//    "safe" marked safe_source
import Basics
import cpp
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.controlflow.Guards

predicate safe_source_with_bless(DataFlow::Node n) {
  exists(FieldAccess f |
    exists(Type t |
      t.getName() = "Config" and
      f.getQualifier().getType().resolveTypedefs().refersTo(t)
    ) and
    n.asExpr() = f and
    f.getTarget().getName() = "staff"
  )
}

predicate bless_safe_source(DataFlow::Node n) {
  exists(Parameter p |
    (n.asParameter() = p or n.asExpr() = p.getAnAccess()) and
    p.getFunction().getName() = "questions_submit" and
    p.getIndex() = 1
  )
}

Call email_call() {
  forall(Call c, Function f |
    c.getTarget() = f and
    f.getName() = "send" and
    f.getNamespace().getName() = "email"
  |
    result = c
  )
}

module SinkFlowConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) { any() }

  predicate isSink(DataFlow::Node n) { any() }

  predicate isAdditionalFlowStep(DataFlow::Node n1, DataFlow::Node n2) {
    exists(FieldAccess a, Type sens |
      a.getQualifier().getType().resolveTypedefs().refersTo(sens) and
      is_sensitive(sens) and
      n2.asExpr() = a and
      a.getQualifier() = n1.asExpr()
    )
  }
}

module SinkFlow = TaintTracking::Global<SinkFlowConfig>;

predicate is_scope(DataFlow::Node n) { email_call().getArgument(0) = n.asExpr() }

predicate is_external_sink(DataFlow::Node n) { email_call().getArgument(3) = n.asExpr() }

predicate is_sensitive_node(DataFlow::Node n) { contains_sensitive_type(n.getType()) }

from DataFlow::Node src, DataFlow::Node sink
where is_sensitive_node(src) and is_external_sink(sink) and SinkFlow::flow(src, sink)
select src, sink
//
// from FieldAccess f
// where
//   //   exists(Type t |
//   //     t.getName() = "Config" and
//   //     f.getQualifier().getType().refersTo(t)
//   //   ) and
//   f.getTarget().getName() = "staff"
// select f, f.getQualifier().getType().resolveTypedefs(), f.getLocation()
//
// Bless with safe source condition
// from DataFlow::Node bless_safe_source, DataFlow::Node safe_source_with_bless
// where
//   exists(GuardCondition cond |
//     cond.controls(safe_source_with_bless.asExpr().getBasicBlock(), _) and
//     bless_safe_source.asExpr() = cond.getAChild*()
//   ) and
//   safe_source_with_bless(safe_source_with_bless) and
//   bless_safe_source(bless_safe_source)
// select bless_safe_source, safe_source_with_bless
