#!/usr/bin/env bash

ARG_DEFS=(
    "[--artifactory-token=(.*)]"
    "[--product-version=(.*)]"
    "[--ansible-vault-pw=(.*)]"
    "[--test-tags=(.*)]"
)

function init() {
  export ENVIRONMENT=dev

  # Skip the pipeline if the release type is included in a black list
  skip_release_types "skip" "docs"

  export ARTIFACTORY_TOKEN=${ARTIFACTORY_TOKEN:-xxx}
  export PRODUCT_VERSION=${PRODUCT_VERSION:-$(get_next_version)}
  export ANSIBLE_VAULT_PW=${ANSIBLE_VAULT_PW:-}
  export TEST_TAGS=${TEST_TAGS:-}
  echo "# Product Version: $PRODUCT_VERSION"
}

function run() {
  bash delivery/scripts/build.sh
  bash delivery/scripts/package.sh --product-version=${PRODUCT_VERSION}
  bash delivery/scripts/publish.sh --environment=${ENVIRONMENT} --product-version=${PRODUCT_VERSION}
  bash delivery/scripts/deploy.sh --environment=${ENVIRONMENT} --product-version=${PRODUCT_VERSION} --ansible-vault-pw=${ANSIBLE_VAULT_PW} --test-tags=${TEST_TAGS}
  bash delivery/scripts/promote.sh --environment=${ENVIRONMENT} --product-version=${PRODUCT_VERSION}

  [ "$BUILD_LATEST" == "true" ] && bash delivery/pipelines/latest.sh
}

source $(dirname $0)/../base.inc