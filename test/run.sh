#!/bin/sh

RUN_DIR=$(dirname $0)

echo "${RUN_DIR}/lang/run.sh"
${RUN_DIR}/lang/run.sh "$@"

printf "\n\n"

echo "${RUN_DIR}/cmd/run.sh"
${RUN_DIR}/cmd/run.sh "$@"



