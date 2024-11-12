private import cpp
private import semmle.code.cpp.dataflow.new.TaintTracking

predicate is_sensitive(Type t) {
  exists(string s |
    t.getName() = s and
    (
      s = "ApiKey" or
      s = "LectureQuestionSubmission" or
      s = "LectureQuestion" or
      s = "LectureAnswer"
    )
  )
}

predicate contains_sensitive_type(Type t) {
  exists(Type s |
    is_sensitive(s) and
    t.resolveTypedefs().refersTo(s)
  )
}

predicate is_store_function(Function f) {
  (
    f.getName() = "replace" or
    f.getName() = "insert"
  ) and
  f.getDeclaringType().getName() = "MySqlBackend"
}

predicate is_store(DataFlow::Node n) {
  exists(Variable v |
    n.asVariable() = v and
    is_store_function(v.getEnclosingElement().(Function)) and
    v.getName() = "reval"
  )
  //   exists(Parameter p |
  //     n.asParameter() = p and p.getIndex() = 1 and is_store_function(p.getFunction())
  //   )
}

// predicate is_store(Expr e) {
//   exists(FunctionCall c |
//     (
//       c.getTarget().getName() = "replace" or
//       c.getTarget().getName() = "insert"
//     ) and
//     c.getArgument(1) = e
//   )
// }
module StoreTaintConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) {
    any()
    //exists(Type t | is_sensitive(t) and n.getType().refersTo(t))
  }

  predicate isSink(DataFlow::Node n) {
    any()
    //is_store(n.asExpr())
  }
}

module StoreTaint = TaintTracking::Global<StoreTaintConfig>;

module AllFlowsConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node n) { any() }

  predicate isSink(DataFlow::Node n) { any() }
}

module AllFlows = TaintTracking::Global<AllFlowsConfig>;
