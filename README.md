## Setup

The following steps assume that you are using Apple-silicon MacOS and that you
are installing the codeql bundle into a sibling directory of this repository
called `codeql-home`. I include links to the documentation where these steps
originate from so you can adjust them if these assumptions don't hold for you.

These first steps are from [this codeql documentation](https://docs.github.com/en/code-security/codeql-cli/getting-started-with-the-codeql-cli/setting-up-the-codeql-cli).

Download the MacOS codeql release and unpack it.

```bash
wget https://github.com/github/codeql-action/releases/download/codeql-bundle-v2.19.0/codeql-bundle-osx64.tar.gz
tar -xvf codeql-bundle-osx64.tar.gz
rm codeql-bundle-osx64.tar.gz
mv codeql ../codeql-home
```

Then create an alias for the codeql command. You have to repeat this step evey
time you restart your shell. This is not taken from the docs. If you use
different paths, set the alias manually or edit the env file.

```bash
source env.sh
```

## Basic Workflow

We are going to assume you are using the `cpp/simple-flow` project. If you are
running a different one, adjust as necessary. Change into the directory with
`cd cpp/simple-flow`.

To run a query you must first create a codeql database. We are going to call the
database `qdb`. The database must be recreated on every code change so if one
already exists first `rm -r qdb`. Then create a new database with `codeql
database create qdb --language cpp`. More info also in this part of [the
documentation](https://docs.github.com/en/code-security/codeql-cli/getting-started-with-the-codeql-cli/preparing-your-code-for-codeql-analysis).

After the database has been created you can run a query. If our query is
`print-all-flows.ql` in the `query` pack, then run `codeql query run
../query/print-all-flows.ql`.

## Creating new queries

The codeql executable can run single query files, hoever it appears that if you
want to use modules from the standard library (such as the `cpp` module) then
you need to create a query pack. Extensive information is available in the
[documentation](https://docs.github.com/en/code-security/codeql-cli/using-the-advanced-functionality-of-the-codeql-cli/creating-and-working-with-codeql-packs)
but what follows are some simple steps that might suffice for e.g. a simple C++
query.

To create a pack named `my-pack` run `codeql pack init my-pack`. Then add
dependencies to it. For instance if you want to add the stdlib for working
with C++ first change into the pack directory with `cd my-pack` then run `codeql
pack add codeql/cpp-all`.
