#!/bin/bash

function usage() {
  echo "$0 [file.java]"
  exit 1
}

if [ $# -lt 1 ]; then
  usage
  exit 1
fi

printf "%s\n%s" "$(cat copyright)" "$(cat $1)" > $1

echo Done
