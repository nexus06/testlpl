function create_release {
  local previous_global_content=$(get_wiki_page ${REDMINE_GLOBAL_RELEASE_PAGE})
  [[ ${previous_global_content} =~ ^.*${PRODUCT_VERSION}.*$ ]] || \
    put_wiki_page ${REDMINE_GLOBAL_RELEASE_PAGE} "$(get_release false)\n\n ${previous_global_content}"
  local release_page=$(echo ${PRODUCT_VERSION} | tr -d .)
  put_wiki_page ${release_page} "$(get_release)"
}

function get_wiki_page() {
  local page_id=${1-${REDMINE_GLOBAL_RELEASE_PAGE}}
  curl -s --insecure -X GET \
    -H "Content-Type: application/json" \
    -H "X-Redmine-API-Key: ${REDMINE_TOKEN}" \
    "${REDMINE_URL}/projects/${REDMINE_PROJECT}/wiki/${page_id}.json" | jq '.wiki_page.text' | sed 's/^"\(.*\)"$/\1/'
}

function put_wiki_page() {
  local page_id=${1-${REDMINE_GLOBAL_RELEASE_PAGE}}
  local content=$2
  local body='{"wiki_page":{"text": "'${content}'"}}'
  curl -s --insecure -X PUT \
    -H "Content-Type: application/json" \
    -H "X-Redmine-API-Key: ${REDMINE_TOKEN}" \
    --data "${body}" \
    "${REDMINE_URL}/projects/${REDMINE_PROJECT}/wiki/${page_id}.json"
}

function get_release() {
  local long=${1-true}
  local notes="h3. *Version:* [[${PRODUCT_VERSION}]] \n"
  [[ ${long} == true ]] && notes="${notes}\n\n-----------"
  notes="${notes}\n* *Release date:* $(date +%d-%m-%Y\ \%H:%M:%S)"
  notes="${notes}\n* *Git tag:* ${PRODUCT_VERSION}"
  [[ ${long} == true ]] && notes="${notes}\n* *Target commitish :* $(get_long_sha)"
  [[ ${long} == true ]] && notes="${notes}\n* *Artifactory :* $(get_shared_var "package_uri")"
  [[ ${long} == true ]] && notes="${notes}\n* *Release Notes:*\n <pre>$(get_release_notes)</pre>"
  [[ ${long} != true ]] && notes="${notes}\n\n-----------"
  echo ${notes}
}
