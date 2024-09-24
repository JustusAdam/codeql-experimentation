#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
CODEQL="$(realpath "$SCRIPT_DIR/../codeql-home/codeql")"

alias codeql="$CODEQL"
