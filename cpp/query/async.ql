import cpp
import FlowConfigs

predicate isAdditionalFlowStepImpl(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall async, int index |
    async.getTarget().getName() = "async" and
    (
      async.getArgument(index + 1) = node1.asExpr() and
      exists(Parameter p |
        node2.asParameter() = p and
        p.getFunction().getName() = "a_function" and
        p.getIndex() = index
      )
      // or
      // exists(DataFlow::Node ptr, DataFlow::Node result_ptr |
      //   ptr.asExpr() = thread.getArgument(1) and
      //   ptr.asIndirectExpr() = node1.asExpr() and
      //   is_a_function_parameter(result_ptr) and
      //   result_ptr.asIndirectExpr() = node2.asExpr()
      // )
      // or
      // exists(DataFlow::Node ptr, DataFlow::Node result_ptr |
      //   ptr.asExpr() = thread.getArgument(1) and
      //   ptr.asIndirectExpr() = node2.asExpr() and
      //   is_a_function_parameter(result_ptr) and
      //   result_ptr.asIndirectExpr() = node1.asExpr()
      // )
    )
  )
}

predicate isTargetFunction(Function f) { f.getName() = "target" }

predicate isTargetParameter(Parameter p) {
  isTargetFunction(p.getFunction()) and
  p.getIndex() = 0
}

predicate isTargetOperand(Expr o) {
  exists(FunctionCall call | isTargetFunction(call.getTarget()) and call.getArgument(0) = o)
}

module MyConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    //isSourceCall(source.asExpr())
    any()
  }

  predicate isSink(DataFlow::Node sink) {
    exists(Call c |
      c.getTarget().getName() = "target" and
      c.getArgument(0) = sink.asExpr()
    )
    // isTargetOperand(sink.asExpr())
  }

  predicate isAdditionalFlowStep(DataFlow::Node node1, DataFlow::Node node2) {
    isAdditionalFlowStepImpl(node1, node2)
  }
}

module MyConfig2 implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) { isSourceCall(source.asExpr()) }

  predicate isSink(DataFlow::Node sink) {
    any() //isTargetOperand(sink.asExpr())
  }

  predicate isAdditionalFlowStep(DataFlow::Node node1, DataFlow::Node node2) {
    isAdditionalFlowStepImpl(node1, node2)
  }
}

module Flow = TaintTracking::Global<MyConfig>;

module Flow2 = TaintTracking::Global<MyConfig2>;

from DataFlow::Node node1, DataFlow::Node node2, Location loc
where
  Flow::flow(node1, node2) and
  loc.getFile().getBaseName() = "main.cpp" and
  node1.getLocation() = loc
select node1, node2, loc.getStartLine(), node1.asExpr().getPrimaryQlClasses()
