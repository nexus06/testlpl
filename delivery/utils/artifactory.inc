function push_apt() {
  local package_name=$(find_apt_package_name $1)
  [ -z "$package_name" ] && return 1
  local repo=${2:-dev}
  local component=${2:-dev}
  local distribution=${3:-trusty}
  local architecture=${4:-all}

  local download_uri=$(curl -s -u ${ARTIFACTORY_TOKEN} -X PUT \
    "http://artifactory.hi.inet/artifactory/${ARTIFACTORY_APT_REPO}/${repo}/${package_name};deb.distribution=${distribution};deb.component=${component};deb.architecture=${architecture}" \
    -T "dist/${package_name}" | jq -r '.["downloadUri"]')
  [ -z "$download_uri" ] && return 1 || echo ${download_uri}
}

function push_play_module() {
   local package_name=$(find_misc_package_name $1)
   [ -z "$package_name" ] && return 1
   local repo=${2:-dev}

   local download_uri=$(curl -s -u ${ARTIFACTORY_TOKEN} -X PUT \
     "http://artifactory.hi.inet/artifactory/${ARTIFACTORY_PLAY_REPO}/${repo}/${package_name}" \
     -T "dist/${package_name}" | jq -r '.["downloadUri"]')
   [ -z "$download_uri" ] && return 1 || echo ${download_uri}
}

function find_apt_package_name() {
  local app_name=$1;
  local filename=$(find . -name "${app_name}*.deb") && echo ${filename##*/} || return 1
}

function find_misc_package_name() {
  local app_name=$1;
  local filename=$(find . -name "${app_name}*.zip") && echo ${filename##*/} || return 1
}
