function gerrit_create_tag() {
  local version=$1
  if [ "$(git describe --tags)" != "${version}" ] ; then
    git tag -a ${version} -m "${version}" && git push origin ${version} HEAD:refs/heads/master
  fi
}