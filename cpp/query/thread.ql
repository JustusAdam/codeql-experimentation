import cpp
import FlowConfigs

module MyConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    any()
    //isSourceCall(source.asExpr())
  }

  predicate isSink(DataFlow::Node sink) { isTargetOperand(sink.asOperand()) }

  predicate isAdditionalFlowStep(DataFlow::Node node1, DataFlow::Node node2) {
    node1.asOperand().getUse().(CallInstruction).getStaticCallTarget().getName() = "thread" and
    node1.asOperand().(PositionalArgumentOperand).getIndex() > 0 and
    // node1.asParameter().getIndex() > 0 and
    node2.asParameter().getFunction().getName() = "a_function"
    or
    node2.asOperand().getUse().(CallInstruction).getStaticCallTarget().getName() = "thread" and
    // node1.asOperand().getUse() instanceof ReturnInstruction and
    node1.getFunction().getName() = "a_function"
  }
}

module Flow = TaintTracking::Global<MyConfig>;

from DataFlow::Node source //, DataFlow::Node sink
where source.asOperand().getUse().(CallInstruction).getStaticCallTarget().getName() = "thread" // and
//source.asOperand().(PositionalArgumentOperand).getIndex() > 0
//source.getFunction().getName() = "a_function"
//Flow::flow(source, sink) //and source != sink
select source, source.asExpr().getAPrimaryQlClass() //, sink, sink.asExpr().getAPrimaryQlClass()
