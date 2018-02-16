#!/usr/bin/env bash

ARG_DEFS=(
  "[--app-name=(.*)]"
  "[--mode=(.*)]"
)

function init() {
  export PRODUCT_VERSION=${PRODUCT_VERSION:-$(get_next_version)}  
 
}

function run() {
 echo "version:$PRODUCT_VERSION"
}

source $(dirname $0)/../base.inc
