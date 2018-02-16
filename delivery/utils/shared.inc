function set_shared_var() {
   local key=$1
   local value=$2
   echo $(get_shared_json | jq '.'$key'="'$value'"') > target/shared.json
}

function get_shared_var() {
   local key=$1
   get_shared_json | jq -r '.'$key'?'
}

function get_shared_json() {
   [ -f "target/shared.json" ] || echo "{}" > target/shared.json && cat target/shared.json
}