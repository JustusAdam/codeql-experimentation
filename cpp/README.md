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
| `basic_string` | integer flows into string (via cast to char) | ❌ |
| `basic_string_write` | integer written to `stringstream` | ✅ |

## For our policies

| Application | Policy | Result | Folder | Query |
|-|-|-|-|-|
| Plume | Data Deletion | ✅ (fails if we require the retrievals to be db methods) | `plume` | `plume-data-deletion.ql` |
| Lemmy | Instance Ban/Delete | ✅ | `lemmy` | `lemmy-instance.ql` |
| Atomic | Authorization | ? | `atomic` | `atomic.ql` |

We presume the Lemmy Community policy would also succeed, as it is virtually the
same as the instance policy.

## A word on building/codeql with libraries 

Many examples (`plume`, `freedit`, `external-library`) use "third party
libraries", which in this case means stub libraries for functionality that would
normally be in a shared or otherwise precompiled external library. We link
those libraries very simply in cmake using the path. What this means is that
both to build the example or run it through codeql you need to first build the
library in a very specific way in a subdirectory of the example called `build`.
For instance the entire codeql workflow for `plume` is as follows:

```sh
cd plume/plib
mkdir build
cd build
cmake ..
cmake --build .
cd ..
codeql database create -lcpp qdb 
# ... run queries
```

For just building replace the codeql command with cmake commands.

As an aside all the libraries are build with x86_64 architecture because this is
what codeql requires, even if you are on Apple silicon.