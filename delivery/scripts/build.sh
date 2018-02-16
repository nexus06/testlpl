#!/usr/bin/env bash

ARG_DEFS=(
  "[--app-name=(.*)]"
  "[--mode=(.*)]"
)

function init() {
#TODO git submodule update --init --recursive
#TODO import project dependence modules
 echo "VERSION: $VERSION"
 echo "GIT_TAG: $GIT_TAG"
 echo "GIT_BRANCH: $GIT_BRANCH"
 echo "DIR": $(dirname $0)
}

function run() {
  #play_precompile ${APP_NAME} ${MODE}
./gradlew assembleRelease --no-daemon
}

source $(dirname $0)/../base.inc
