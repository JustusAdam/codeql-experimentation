import cpp
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.pointsto.PointsTo

predicate isSourceCall(Expr e) { e.(FunctionCall).getTarget().getName() = "source" }

module TaintConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { isSourceCall(source.asExpr()) }

  predicate isSink(DataFlow::Node sink) {
    //is_other_target(sink.asIndirectExpr())
    any()
  }

  predicate isAdditionalFlowStep(DataFlow::Node n1, DataFlow::Node n2) {
    propagate_field_to_struct(n1.asExpr(), n2.asIndirectExpr()) or
    propagate_field_to_struct(n1.asExpr(), n2.asExpr())
  }
}

predicate propagate_field_to_struct(Expr field, Expr struct) {
  exists(Assignment a |
    field = a.getRValue() and
    get_qualifier(a.getLValue()) = struct
  )
  //   and
  //   (is_vec(struct.getType()) or struct.getType().getName() = "with_field")
}

Expr get_qualifier(Expr e) {
  exists(FieldAccess f | f = e and result = f.getQualifier())
  or
  exists(ImplicitThisFieldAccess f | f = e and result = f.getQualifier())
}

predicate is_other_target(Expr e) {
  exists(Call c |
    c.getTarget().getName() = "other_target" and
    c.getArgument(0) = e
  )
}

predicate is_vec(Type t) { t.(ClassTemplateInstantiation).getSimpleName() = "vec" }

module Flow = TaintTracking::Global<TaintConfig>;

// all flows
// from DataFlow::Node n, DataFlow::Node sink, int src_loc, int sink_loc
// where
//   Flow::flow(n, sink) and
//   src_loc = n.getLocation().getStartLine() and
//   sink_loc = sink.getLocation().getStartLine()
// select n, src_loc, sink, sink_loc
//
// all sources and sinks
// from DataFlow::Node n, string message
// where
//   TaintConfig::isSource(n) and message = "source"
//   or
//   TaintConfig::isSink(n) and message = "sink"
// select n, message, n.getLocation().getStartLine()
//
// Where the additional flow step applies
// from DataFlow::Node n, DataFlow::Node sink, int src_loc, int sink_loc
// where
//   TaintConfig::isAdditionalFlowStep(n, sink) and
//   src_loc = n.getLocation().getStartLine() and
//   sink_loc = sink.getLocation().getStartLine() and
//   n.getLocation().getFile().getBaseName() = "main.cpp"
// select n, src_loc, sink, sink_loc
//
// all assignments
// from Assignment a, Expr lvalue, Expr rvalue
// where
//   a.getLocation().getFile().getBaseName() = "main.cpp" and
//   lvalue = a.getLValue() and
//   rvalue = a.getRValue()
// select lvalue, lvalue.getPrimaryQlClasses(), a, rvalue, rvalue.getPrimaryQlClasses()
//
// all dataflow nodes in the main function
// from DataFlow::Node n
// where n.getFunction().getName() = "main"
// select n, n.getLocation().getStartLine()
//
// flows from source
// from DataFlow::Node n, DataFlow::Node sink, int src_loc, int sink_loc, string message
// where
//   Flow::flow(n, sink) and
//   src_loc = n.getLocation().getStartLine() and
//   sink_loc = sink.getLocation().getStartLine() and
//   (if is_other_target(sink.asIndirectExpr()) then message = "is sink" else message = "")
// select n, src_loc, sink, sink_loc, message
//
// points to sets
from Element src, Element dest, Function main_fn
where
  main_fn.getName() = "main" and
  src.toString() != dest.toString() and
  pointer(src, dest) and
  src.getLocation().getFile().getBaseName() = "main.cpp"
select src, dest
