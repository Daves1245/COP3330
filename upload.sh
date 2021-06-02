#!/bin/bash

function usage() {
  echo "$0 [git repo]"
}

if [ $# -le 2 ]; then
  usage
fi

# Just push all the files to their repo.
cd $1
git add -A
git commit -m "Upload files"
git push

exit
