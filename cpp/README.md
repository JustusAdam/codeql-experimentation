# Results for C++

The `query` directory contains a codeql pack with the queries I tried on this.
Mostly the `simple-source-sink.ql` query.

| Experiment | Description | Result |
|-|-|-|
| `simple-flow` | A simple, global data flow problem | ✅ |
| `function-pointer` | Data-flow via a function pointer | ✅ |
| `hof` | Data-flow involving a higher order function | ✅ |
