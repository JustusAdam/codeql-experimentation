# Results for C++

The `query` directory contains a codeql pack with the queries I tried on this.
Mostly the `simple-source-sink.ql` query.

| Experiment | Description | Result |
|-|-|-|
| `simple-flow` | A simple, global data flow problem | ✅ |
| `function-pointer` | Data-flow via a function pointer | ✅ |
| `function-pointer-2` | Higher-order function with indirect assignment | ❌ |
| `function-pointer-2-templatized` | `function-pointer-2` using templates for theoretically precise type (closer to Rust) | ❌ |
| `hof` | Data-flow involving a higher order function | ✅ |
| `uninstantiated-template` | Analyzing templated entry points | ❌ |
| `dynamic-dispatch` | Analyzing virtual function calls | ❌ |
| `vector` | Dataflow through a vector element | ❌ |
| `thread` | Marker behind `std::thread` | ❌ |
| `context` | Attempting to get points-to contexts confused by layering functions | ✅ |
| `plume` | Data deletion policy and application like Plume | ❌ |
