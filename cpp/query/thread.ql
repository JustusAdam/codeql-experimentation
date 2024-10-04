import cpp
import FlowConfigs

predicate isAdditionalFlowStepImpl(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall thread |
    thread.getTarget().getName() = "thread" and
    (
      thread.getArgument(1) = node1.asExpr() and is_a_function_parameter(node2)
      or
      thread.getArgument(1) = node2.asExpr() and is_a_function_parameter(node1)
      or
      exists(DataFlow::Node ptr, DataFlow::Node result_ptr |
        ptr.asExpr() = thread.getArgument(1) and
        ptr.asIndirectExpr() = node1.asExpr() and
        is_a_function_parameter(result_ptr) and
        result_ptr.asIndirectExpr() = node2.asExpr()
      )
      or
      exists(DataFlow::Node ptr, DataFlow::Node result_ptr |
        ptr.asExpr() = thread.getArgument(1) and
        ptr.asIndirectExpr() = node2.asExpr() and
        is_a_function_parameter(result_ptr) and
        result_ptr.asIndirectExpr() = node1.asExpr()
      )
    )
  )
}

module MyConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    //isSourceCall(source.asExpr())
    any()
  }

  predicate isSink(DataFlow::Node sink) { isTargetOperand(sink.asExpr()) }

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

predicate is_a_function_parameter(DataFlow::Node n) {
  n.asParameter().getFunction().getName() = "a_function"
}

module Flow = TaintTracking::Global<MyConfig>;

module Flow2 = TaintTracking::Global<MyConfig2>;

from DataFlow::Node node1, DataFlow::Node node2, string message
where isAdditionalFlowStepImpl(node1, node2) and message = "placeholder"
//   if Flow::flow(node1, node2)
//   then message = "to sink"
//   else (
//     Flow2::flow(node1, node2) and
//     message = "from source"
//   )
//source.asOperand().getUse().(CallInstruction).getStaticCallTarget().getName() = "thread" // and
//source.asOperand().(PositionalArgumentOperand).getIndex() > 0
//source.getFunction().getName() = "a_function"
//Flow::flow(source, sink) //and source != sink
select node1, node2, message
