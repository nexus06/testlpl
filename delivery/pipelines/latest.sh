#!/usr/bin/env bash

ARG_DEFS=(
    "[--artifactory-token=(.*)]"
)

function init() {
  export ARTIFACTORY_TOKEN=${ARTIFACTORY_TOKEN:-xxx}
  export ENVIRONMENT=${ENVIRONMENT:-dev}
  export PRODUCT_VERSION=latest
  echo "# Product Version: $PRODUCT_VERSION"
}

function run() {
  bash delivery/scripts/build.sh
  bash delivery/scripts/package.sh --product-version=${PRODUCT_VERSION}
  bash delivery/scripts/publish.sh --product-version=${PRODUCT_VERSION} --environment=${ENVIRONMENT}
  bash delivery/scripts/promote.sh --product-version=${PRODUCT_VERSION} --environment=${ENVIRONMENT}
}

source $(dirname $0)/../base.inc