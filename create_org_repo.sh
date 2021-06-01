#!/bin/bash

function usage() {
    echo "$0 [oauth_token filepath] [organization name] [repo name]"
    exit 1
}

if [ $# -lt 3 ]; then
    usage
fi

TOKEN_PATH=$1
ORGNIZATION=$2
REPO_NAME=$3
full_token=$(cat $TOKEN_PATH)
token=$(echo $full_token | cut -c 19-58)

curl \
  -X POST \
  -H "Authorization: token $token" \
  https://api.github.com/orgs/$ORGANIZATION/repos \
  -d "{\"name\":\"$REPO_NAME\"}"

exit 0
