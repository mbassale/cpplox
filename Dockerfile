FROM ubuntu:22.04

ENV HOME /root

SHELL ["/bin/bash", "-c"]

RUN apt-get update && apt-get -y --no-install-recommends install \
  build-essential \
  clang \
  cmake \
  gdb \
  wget \
  git \
  unzip \
  curl

RUN apt-get -y install libgtest-dev libgflags-dev libgoogle-glog-dev
