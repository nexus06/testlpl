#!/usr/bin/env bash

ARG_DEFS=(
  "[--app-name=(.*)]"
  "[--mode=(.*)]"
)

declare -a RELEASE_TYPES=(unk feat fix docs style refactor perf test chore skip)

function get_version() {
  echo ${PRODUCT_VERSION:-$(get_last_tag_version || echo "0.0.0")}
}

function is_prerelease() {
  local version=${1:-$(get_version)}
  if [[ $version == *"SNAPSHOT"* ]]; then
    return 0
   fi
  local prerelease=$(echo $version | cut -d '-' -f 2)
  [ "$prerelease" != "$version" ] && return 0 || return 1
}

function fetch_tags() {
  [ "$_TAGS_FETCHED" == "0" ] && git fetch --tags && _TAGS_FETCHED=1 || return 0
}

function get_last_tag_version() {
  fetch_tags
  local last_tag=""
  local tags=$(git log --tags --pretty="format:%D" | grep "tag: " | awk -F "tag: " '{print $2}' | cut -d ',' -f 1) 
  shopt -s extglob
  for tag in $tags; do
    [[ ${tag} =~ ^v?[0-9]+\.[0-9]+\.[0-9]+.*$ ]] && last_tag=$tag; break;
  done
  [ -z "${last_tag}" ] && return 1 || echo ${last_tag}
}

function is_release_a_feature() {
  local release_type=$(get_release_type 2>/dev/null)
echo release_type
  ([ "$release_type" == "" ] || [ "$release_type" == "unk" ] || [ "$release_type" == "feat" ]) && return 0 || return 1
}

function get_commit_type() {
  local commit=$1
  local commit_type=$(echo "$commit" | cut -s -d ':' -f 1 | cut -s -d ' ' -f 2)
  for i in "${RELEASE_TYPES[@]}"; do
    [ "$i" == "$commit_type" ] && echo "$commit_type" && return 0
  done
}

function get_next_version_from_feature() {
  local version=$(get_last_tag_version || echo '0.0.0')
  IFS='.' read -r -a array <<< "${version}"
  is_release_a_feature && echo ${array[0]}.$((array[1]+1)).0 || echo ${array[0]}.${array[1]}.$((array[2]+1))
}

function get_release_type() {
  local release_type=""
  while read -r commit; do
    local commit_type=$(get_commit_type "${commit}")
    release_type=$(get_highest_commit_type "$release_type" "$commit_type")
  done <<< "$(get_release_commits)"
  echo $release_type
}

function get_release_commits() {
  if [ -n "$(get_last_tag_version)" ]; then
    git log $(get_last_tag_version)..HEAD --oneline;
  else
    git log HEAD --oneline;
  fi
}

function get_highest_commit_type() {
  release_type=$1
  commit_type=$2
  for i in "${RELEASE_TYPES[@]}"; do
    [ "$i" == "$commit_type" ] || [ "$i" == "$release_type" ] && echo "$i" && return 0
  done
  echo "unk"
}


function get_next_version() {
  is_prerelease && get_version || get_next_version_from_feature
}
 
echo ${PRODUCT_VERSION:-$(get_next_version)}  

