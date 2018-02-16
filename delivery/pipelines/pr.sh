#!/usr/bin/env bash

ARG_DEFS=(
   "[--app-root=(.*)]"
   "[--test-tags=(.*)]"
)

function init() {
  export ENVIRONMENT=local
  export APP_ROOT=${APP_ROOT:-/opt/darwin/sample/darwin-app}
  export TEST_TAGS=${TEST_TAGS:-}
}

function run() {
  bash delivery/scripts/build.sh
  #play_start ${APP_ROOT} ${ENVIRONMENT} || true
  #cd delivery/deploy
  #bash ../scripts/test-e2e.sh --environment=${ENVIRONMENT} --test-tags=${TEST_TAGS} && \
  #  local status="success" || local status="failure"
  #play_stop ${APP_ROOT} || true
}

source $(dirname $0)/../base.inc
