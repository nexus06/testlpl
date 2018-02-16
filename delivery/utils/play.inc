function set_play_home() {
    export PLAY_HOME=$(which play | sed -e 's/play-.*\/play/play-'${PLAY_VERSION}'/g')
    if [ ! -d "${PLAY_HOME}" ]; then
      read -r -p "# Play! ${PLAY_VERSION} is not installed do you want install it?[Y/N]" response
      case "$response" in
           [yY][eE][sS]|[yY])
               local absolute_play_home=$(echo ${PLAY_HOME} | sed -e 's/play-.*//g')
               echo "# Play! installation dir : ${absolute_play_home}"
               wget -qO- -O ${PLAY_HOME}.zip "https://downloads.typesafe.com/play/${PLAY_VERSION}/play-${PLAY_VERSION}.zip" && \
                unzip ${PLAY_HOME}.zip -d ${absolute_play_home} && \
                rm ${PLAY_HOME}.zip
               ;;
           *) echo "# [WARN] Play! ${PLAY_VERSION} is not installed." ;;
      esac
    fi
}

# Precompile app passing the framework id (mode)
# The is not precompiled if contains compilation errors.
function play_precompile() {
  local name=${1:-play-app}
  local mode=${2:-}
  set_play_home
  play_clean
  play_deps
  if [ -z ${mode} ]; then
    ${PLAY_HOME}/play precompile --deps;
  else
    ${PLAY_HOME}/play precompile --deps --%${MODE};
  fi
  play_target ${name}
}

# Build the target files needed to package app
function play_target() {
  local name=${1:-play-app}
  rm -rf target
  mkdir -p target/${name}
  for target in conf precompiled public lib modules ; do
    if [ -d "${target}" ]; then
      echo "# Packaging... ${target}"
      cp -r ${target} target/${name} 2>/dev/null || :
    fi
    if [ -f "${target}" ]; then
      echo "# Packaging file... ${target}"
      cp ${target} target/${name}
    fi
  done
  play_relativize_modules target/${name}
}

function play_relativize_modules() {
  local app_root=${1:-.}
  local modules_install_dir=${2:-/opt}
  if [  -d "${app_root}/modules" ]; then
    for module in ${app_root}/modules/*; do
      if [ ! -d "$module" ]; then
        local module_dir=$(cat $module)
        local new_module_dir="${modules_install_dir}/$(echo ${module_dir##*/})"
        echo ${new_module_dir} > ${module} \
            && echo "# New relative module... ${module_dir} => ${new_module_dir}"
      fi
    done
  fi
}

function play_target_module() {
  local name=${1:-play-app}
  rm -rf target
  mkdir -p target/${name}
  for target in app conf public lib test build.xml commands.py Dockerfile ; do
    if [ -d "${target}" ]; then
      echo "# Packaging... ${target}"
      cp -r ${target} target/${name} 2>/dev/null || :
    fi
    if [ -f "${target}" ]; then
      echo "# Packaging file... ${target}"
      cp ${target} target/${name}
    fi
  done
  rm -rf target/${name}/test/acceptance/src/venv || true
  play_set_module_version ${PRODUCT_VERSION} target/${name}
}

function play_set_module_version() {
  local module_version=${1:-${PRODUCT_VERSION}}
  local app_root=${2:-.}
  if [  -f "${app_root}/conf/dependencies.yml" ]; then
    echo "# Setting module version to ${PRODUCT_VERSION}..."
    sed -i -e 's/@PRODUCT_VERSION@/'${module_version}'/g' ${app_root}/conf/dependencies.yml
    if [ -f ${app_root}/conf/dependencies.yml-e ]; then
        rm ${app_root}/conf/dependencies.yml-e
    fi
  fi
}

function play_clean() {
  rm -rf -- precompiled dist .gradle
  set_play_home
  ${PLAY_HOME}/play clean
}

function play_deps() {
  set_play_home
  ${PLAY_HOME}/play dependencies --sync --clearcache
}

function play_start() {
  set_play_home
  local app_root=${1:-}
  local mode=${2:-}
  if [ -z ${mode} ]; then
    ${PLAY_HOME}/play start ${app_root}
  else
    ${PLAY_HOME}/play start ${app_root} --%${mode};
  fi
}

function play_stop() {
  set_play_home
  local app_root=${1:-}
  ${PLAY_HOME}/play stop ${app_root}
}

function play_package() {
  local app_name=${APP_NAME:-play-app}
  local version=${PRODUCT_VERSION:-0.0.0}
  local app_user=${APP_USER:-ubuntu}
  local app_group=${APP_GROUP:-sudo}
  local prefix=${INSTALL_DIR:-/opt/11paths}
  local play_version=${PLAY_VERSION:-1.4.4}
  local out_dir=dist

  app_name=${app_name} \
  version=${version} \
  app_user=${app_user} \
  app_group=${app_group} \
  prefix=${prefix} \
  play_version=${play_version} \
  erb delivery/utils/templates/deb/play-app.upstart > target/${app_name}.upstart

  app_name=${app_name} \
  version=${version} \
  app_user=${app_user} \
  app_group=${app_group} \
  prefix=${prefix} \
  play_version=${play_version} \
  erb delivery/utils/templates/deb/play-app.systemd > target/${app_name}.service

  mkdir -p ${out_dir}

  fpm -f \
    --deb-user          "${app_user}" \
    --deb-group         "${app_group}" \
    --prefix            "${prefix}" \
    --deb-no-default-config-files \
    -t                  deb \
    -a                  all \
    -n                  "${app_name}" \
    --description       "Telefónica Digial ${app_name} Play! app" \
    -m                  "support@11paths.com" \
    --vendor            "Telefónica Digital" \
    --license           "Proprietary" \
    --version           "${version}" \
    --package           "${out_dir}" \
    -s                  dir \
    -C                  "target/" \
    --template-scripts  \
    --after-install     "delivery/utils/templates/deb/start.sh" \
    --before-remove     "delivery/utils/templates/deb/stop.sh" \
    --after-remove      "delivery/utils/templates/deb/clean.sh" \
    --before-upgrade    "delivery/utils/templates/deb/stop.sh" \
    --after-upgrade     "delivery/utils/templates/deb/start.sh" \
    --deb-upstart       "target/${app_name}.upstart" \
    --deb-systemd       "target/${app_name}.service" \
    --template-value    app_name=${app_name} \
    "${app_name}"
}

function play_package_module() {
  local name=${1:-play-app}
  local app_root=${2:-target/${name}}
  mkdir -p dist

  set_play_home
  play_target_module ${name}
  ${PLAY_HOME}/play build-module ${app_root} --require ${PLAY_VERSION:-1.4.4} && cp ${app_root}/dist/${name}-*.zip dist/
}