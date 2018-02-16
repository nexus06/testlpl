# Check if a Docker image:tag exist
function exist_docker_image() {
  local image=${1:-}
  local tag=${2:-}
  local build_name
  [ -z "$tag" ] && build_name=${image} || build_name=${image}:${tag}
  [ "$(docker images -q ${build_name})" == "" ] && return 1 || return 0
}