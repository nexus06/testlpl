# Get the current version (from PRODUCT_VERSION or the last tag)
function get_version() {
  echo ${PRODUCT_VERSION:-$(get_last_tag_version || echo "0.0.0")}
}
# Get the revision (from PRODUCT_REVISION or the last tag)
function get_revision() {
  echo ${PRODUCT_REVISION:-$(get_last_tag_revision)}
}
# Get the last tag (e.g. 1.1.0). It must correspond with a version!
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
# Get the revision: number of commits since last tag, and hash of this revision.
# For example, if describe returns 1.1.0-67-g6652da2, then revision is 67.g6652da2
function get_last_tag_revision() {
  local last_tag=$(get_last_tag_version)
  IFS='-' read -r -a array <<< "$(git describe --tags --long $last_tag) | rev"
  echo "${array[0]}.${array[1]}" | rev
}
_TAGS_FETCHED=0
# Fetch tags but only once (fetching is slow).
function fetch_tags() {
  [ "$_TAGS_FETCHED" == "0" ] && git fetch --tags && _TAGS_FETCHED=1 || return 0
}
function get_release_notes() {
  local notes=""
  while IFS='' read -r line || [[ -n "$line" ]]; do
    local note=$(echo "$line" | cut -d ' ' -f 2-)
    notes="${notes}\\n - $note"
  done <<< "$(get_release_commits)"
  notes=${notes//\"/\\\"}
  echo $notes
}
# Get the next version using the version in the last tag. If the version corresponds to a pre-release,
# the the version is maintained. Otherwise, the next version is calculated from the types of commit
# (if it is a feature or not).
function get_next_version() {
  is_prerelease && get_version || get_next_version_from_feature
}
# According to semver (http://semver.org/), a version is considered a pre-release if the version
# is appended with a hyphen and an identifier (e.g. 1.0.0-alpha, 1.0.0-alpha.1, 1.0.0-0.3.7)
# It returns 0 if the version corresponds to a pre-release (e.g. 1.0.0-alpha), and 1 otherwise (e.g. 1.0.0).
function is_prerelease() {
  local version=${1:-$(get_version)}
  if [[ $version == *"SNAPSHOT"* ]]; then
    return 0
   fi
  local prerelease=$(echo $version | cut -d '-' -f 2)
  [ "$prerelease" != "$version" ] && return 0 || return 1
}
# Get the next version using the version in the last tag and considering if it is a fix or not
# to increment the third digit or the second one.
function get_next_version_from_feature() {
  local version=$(get_last_tag_version || echo '0.0.0')
  IFS='.' read -r -a array <<< "${version}"
  is_release_a_feature && echo ${array[0]}.$((array[1]+1)).0 || echo ${array[0]}.${array[1]}.$((array[2]+1))
}
# Return 0 if release is a feature (release types: "unk" or "feat").
# Otherwise, it returns 1.
function is_release_a_feature() {
  local release_type=$(get_release_type 2>/dev/null)
  [ "$release_type" == "" ] || [ "$release_type" == "unk" ] || [ "$release_type" == "feat" ] && return 0 || return 1
}
# Get the release type and check if it matches with one of the parameters (release types to be skipped).
# It if matches, it exits with 0 code.
function skip_release_types() {
  local release_type=$(get_release_type 2>/dev/null)
  for skippable_release_type in $@; do
    [ "$skippable_release_type" == "$release_type" ] && echo "Skipping release with type: $release_type" && exit 0
  done
  return 0
}
################################################################################
# Release type functions
# The commits of the release identify the type of change in order to infer
# the next version.
################################################################################
declare -a RELEASE_TYPES=(unk feat fix docs style refactor perf test chore skip)
# Get the commits from the last tag to the HEAD. If no tag yet, then gather all commits.
function get_release_commits() {
  if [ -n "$(get_last_tag_version)" ]; then
    git log $(get_last_tag_version)..HEAD --oneline;
  else
    git log HEAD --oneline;
  fi
}
# Get the release type by checking every commit of the release.
function get_release_type() {
  local release_type=""
  while read -r commit; do
    local commit_type=$(get_commit_type "${commit}")
    release_type=$(get_highest_commit_type "$release_type" "$commit_type")
  done <<< "$(get_release_commits)"
  echo $release_type
}
# Get the predominant commit type for two commit types.
# It requires the following arguments:
# - $1: first commit type
# - $2: second commit type
# Note that "unk" has the highest priority.
function get_highest_commit_type() {
  release_type=$1
  commit_type=$2
  for i in "${RELEASE_TYPES[@]}"; do
    [ "$i" == "$commit_type" ] || [ "$i" == "$release_type" ] && echo "$i" && return 0
  done
  echo "unk"
}
# Get the commit type for a single commit.
# It requires the following arguments:
# - $1: commit (single line)
# The function echoes the commit type (see RELEASE_TYPES).
function get_commit_type() {
  local commit=$1
  local commit_type=$(echo "$commit" | cut -s -d ':' -f 1 | cut -s -d ' ' -f 2)
  for i in "${RELEASE_TYPES[@]}"; do
    [ "$i" == "$commit_type" ] && echo "$commit_type" && return 0
  done
}
# Get long SHA from HEAD.
function get_long_sha() {
  git rev-parse HEAD
}
# Get long SHA from HEAD.
function get_long_sha() {
  git rev-parse HEAD
}
# Tag commit with version
function tag_version() {
  local version=$1
  git tag $version
  git push origin --tags
}# Get the current version (from PRODUCT_VERSION or the last tag)
function get_version() {
  echo ${PRODUCT_VERSION:-$(get_last_tag_version || echo "0.0.0")}
}
# Get the revision (from PRODUCT_REVISION or the last tag)
function get_revision() {
  echo ${PRODUCT_REVISION:-$(get_last_tag_revision)}
}
# Get the last tag (e.g. 1.1.0). It must correspond with a version!
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
# Get the revision: number of commits since last tag, and hash of this revision.
# For example, if describe returns 1.1.0-67-g6652da2, then revision is 67.g6652da2
function get_last_tag_revision() {
  local last_tag=$(get_last_tag_version)
  IFS='-' read -r -a array <<< "$(git describe --tags --long $last_tag) | rev"
  echo "${array[0]}.${array[1]}" | rev
}
_TAGS_FETCHED=0
# Fetch tags but only once (fetching is slow).
function fetch_tags() {
  [ "$_TAGS_FETCHED" == "0" ] && git fetch --tags && _TAGS_FETCHED=1 || return 0
}
function get_release_notes() {
  local notes=""
  while IFS='' read -r line || [[ -n "$line" ]]; do
    local note=$(echo "$line" | cut -d ' ' -f 2-)
    notes="${notes}\\n - $note"
  done <<< "$(get_release_commits)"
  notes=${notes//\"/\\\"}
  echo $notes
}
# Get the next version using the version in the last tag. If the version corresponds to a pre-release,
# the the version is maintained. Otherwise, the next version is calculated from the types of commit
# (if it is a feature or not).
function get_next_version() {
  is_prerelease && get_version || get_next_version_from_feature
}
# According to semver (http://semver.org/), a version is considered a pre-release if the version
# is appended with a hyphen and an identifier (e.g. 1.0.0-alpha, 1.0.0-alpha.1, 1.0.0-0.3.7)
# It returns 0 if the version corresponds to a pre-release (e.g. 1.0.0-alpha), and 1 otherwise (e.g. 1.0.0).
function is_prerelease() {
  local version=${1:-$(get_version)}
  if [[ $version == *"SNAPSHOT"* ]]; then
    return 0
   fi
  local prerelease=$(echo $version | cut -d '-' -f 2)
  [ "$prerelease" != "$version" ] && return 0 || return 1
}
# Get the next version using the version in the last tag and considering if it is a fix or not
# to increment the third digit or the second one.
function get_next_version_from_feature() {
  local version=$(get_last_tag_version || echo '0.0.0')
  IFS='.' read -r -a array <<< "${version}"
  is_release_a_feature && echo ${array[0]}.$((array[1]+1)).0 || echo ${array[0]}.${array[1]}.$((array[2]+1))
}
# Return 0 if release is a feature (release types: "unk" or "feat").
# Otherwise, it returns 1.
function is_release_a_feature() {
  local release_type=$(get_release_type 2>/dev/null)
  [ "$release_type" == "" ] || [ "$release_type" == "unk" ] || [ "$release_type" == "feat" ] && return 0 || return 1
}
# Get the release type and check if it matches with one of the parameters (release types to be skipped).
# It if matches, it exits with 0 code.
function skip_release_types() {
  local release_type=$(get_release_type 2>/dev/null)
  for skippable_release_type in $@; do
    [ "$skippable_release_type" == "$release_type" ] && echo "Skipping release with type: $release_type" && exit 0
  done
  return 0
}
################################################################################
# Release type functions
# The commits of the release identify the type of change in order to infer
# the next version.
################################################################################
declare -a RELEASE_TYPES=(unk feat fix docs style refactor perf test chore skip)
# Get the commits from the last tag to the HEAD. If no tag yet, then gather all commits.
function get_release_commits() {
  if [ -n "$(get_last_tag_version)" ]; then
    git log $(get_last_tag_version)..HEAD --oneline;
  else
    git log HEAD --oneline;
  fi
}
# Get the release type by checking every commit of the release.
function get_release_type() {
  local release_type=""
  while read -r commit; do
    local commit_type=$(get_commit_type "${commit}")
    release_type=$(get_highest_commit_type "$release_type" "$commit_type")
  done <<< "$(get_release_commits)"
  echo $release_type
}
# Get the predominant commit type for two commit types.
# It requires the following arguments:
# - $1: first commit type
# - $2: second commit type
# Note that "unk" has the highest priority.
function get_highest_commit_type() {
  release_type=$1
  commit_type=$2
  for i in "${RELEASE_TYPES[@]}"; do
    [ "$i" == "$commit_type" ] || [ "$i" == "$release_type" ] && echo "$i" && return 0
  done
  echo "unk"
}
# Get the commit type for a single commit.
# It requires the following arguments:
# - $1: commit (single line)
# The function echoes the commit type (see RELEASE_TYPES).
function get_commit_type() {
  local commit=$1
  local commit_type=$(echo "$commit" | cut -s -d ':' -f 1 | cut -s -d ' ' -f 2)
  for i in "${RELEASE_TYPES[@]}"; do
    [ "$i" == "$commit_type" ] && echo "$commit_type" && return 0
  done
}
# Get long SHA from HEAD.
function get_long_sha() {
  git rev-parse HEAD
}
# Get long SHA from HEAD.
function get_long_sha() {
  git rev-parse HEAD
}
# Tag commit with version
function tag_version() {
  local version=$1
  git tag $version
  git push origin --tags
}
