#######################################################################
# Dockerfile to build an Ubuntu Android SDK container image
# Based on Ubuntu 16.04 
# inspired from:
# https://github.com/codepath/android_guides/wiki/Installing-Android-SDK-Tools
# https://ncona.com/2016/07/android-development-with-docker/
# https://hub.docker.com/r/bringnow/android-sdk/~/dockerfile/
# https://hub.docker.com/r/oreomitch/ubuntu-android-sdk/~/dockerfile/
# http://paulsbruce.io/blog/2017/01/installing-android-sdk-in-docker/
#######################################################################

FROM dockerhub.hi.inet/dcip/minimal:ubuntu16.04

MAINTAINER Luis Pulido <luis.pulido@gmail.com>

ENV GRADLE_VERSION 3.1
ENV SDK = r25.2.3

ENV ANDROID_HOME /opt/android-sdk-linux

# Install Dependences
RUN dpkg --add-architecture i386 && apt-get update \
    && apt-get install -y expect wget \
    libc6-i386 lib32stdc++6 lib32gcc1 lib32ncurses5 lib32z1

#Installing Java 
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils && apt-get -y install openjdk-8-jre unzip

#Installing Gradle 
RUN wget -O /tmp/gradle-$GRADLE_VERSION-all.zip http://downloads.gradle.org/distributions/gradle-$GRADLE_VERSION-all.zip \
    && unzip -oq /tmp/gradle-$GRADLE_VERSION-all.zip -d /opt/gradle \
    && rm -f /tmp/gradle-$GRADLE_VERSION-all.zip  \
    && ln -sfnv gradle-$GRADLE_VERSION /opt/gradle/latest

# Install Android SDK installer. Only tools @see https://github.com/codepath/android_guides/wiki/Installing-Android-SDK-Tools
RUN wget -O android-sdk.zip https://dl.google.com/android/repository/tools_r25.2.3-linux.zip \
  && unzip -oq android-sdk.zip -d "${ANDROID_HOME}" \
  && rm *.zip

# Setup PATH
ENV PATH ${PATH}:${ANDROID_HOME}/tools/bin:${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools:/opt/sdk-tools

# Install sdk elements
RUN mkdir /opt/tools
COPY tools /opt/tools
RUN chmod +x /opt/tools/android-accept-licenses.sh
ENV PATH ${PATH}:/opt/tools

#Prepare local repos
RUN mkdir ~/.android && touch ~/.android/repositories.cfg

# keep in this order
#RUN echo y|sdkmanager "tools"
RUN ["/opt/tools/android-accept-licenses.sh", \
    "sdkmanager tools"]
RUN sdkmanager "build-tools;24.0.3"
RUN sdkmanager "build-tools;25.0.2"
RUN sdkmanager "build-tools;26.0.2"
RUN sdkmanager "platform-tools"
RUN sdkmanager "platforms;android-24"
RUN sdkmanager "platforms;android-25"
RUN sdkmanager "extras;android;m2repository"
RUN sdkmanager "extras;google;google_play_services"
RUN sdkmanager --update

# Accept all licenses at once 
RUN yes|sdkmanager --licenses

RUN sdkmanager --list

# Cleaning
RUN apt-get clean
