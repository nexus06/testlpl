#!/bin/bash

export GROUPID="com.telefonica.troya"
export ARTIFACTID="settings"

export ANDROID_HOME=/opt/android-sdk-linux

export GIT_TAG=$(git describe --abbrev=0 --tags 2> /dev/null)
export GIT_BRANCH=$BRANCH
export GIT_COMMIT=$(git rev-parse --short HEAD)
export GIT_COMMIT_SHORT=$(git rev-parse HEAD)
export WORKSPACE=$(dirname $0)
export BUILD_OUT_DIR="$WORKSPACE/app/build/outputs/apk"
export APK_UNSIGNED="$BUILD_OUT_DIR/app-release-unsigned.apk"
export APK_SIGNED="$BUILD_OUT_DIR/app-release-signed.apk"
export APK_SIGNED_ALIGNED="$BUILD_OUT_DIR/app-release-signed-aligned.apk"

if [ -z $GIT_TAG ]
then 
    export BASE_VERSION="0.0.1";
else 
    export BASE_VERSION=$GIT_TAG
fi


if [ "$GIT_BRANCH" = "master" ]
then 
    export ARTIFACT_VERSION=$BASE_VERSION
else 
    export ARTIFACT_VERSION=$BASE_VERSION"-SNAPSHOT"
fi

if [ "$GIT_BRANCH" = "$GIT_TAG" ]
then 
    export ARTIFACT_VERSION=$BASE_VERSION
fi

export OUT_NAME=${GROUPID}.${ARTIFACTID}-${ARTIFACT_VERSION}.apk
