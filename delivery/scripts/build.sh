#!/usr/bin/env bash

ARG_DEFS=(
  "[--app-name=(.*)]"
  "[--mode=(.*)]"
)

function init() {
#TODO git submodule update --init --recursive
#TODO import project dependence modules

 # Skip execution if the release type is included in a black list
 skip_release_types "skip" "docs"
  export ARTIFACTORY_TOKEN=${ARTIFACTORY_TOKEN:-xxx}
  export PRODUCT_VERSION=${PRODUCT_VERSION:-$(get_next_version)}
  export ANSIBLE_VAULT_PW=${ANSIBLE_VAULT_PW:-}
  export TEST_TAGS=${TEST_TAGS:-}
  echo "# Product Version: $PRODUCT_VERSION"
}

function run() {
  #play_precompile ${APP_NAME} ${MODE}
./gradlew assembleRelease --no-daemon
}

source $(dirname $0)/../base.inc
