FROM ubuntu:22.04

ENV HOME /root

SHELL ["/bin/bash", "-c"]

# Install dependencies
RUN apt-get update && apt-get -y --no-install-recommends install \
  build-essential \
  clang \
  cmake \
  gdb \
  wget \
  git \
  openssh-server \
  unzip \
  curl \ 
  libgtest-dev \
  libgflags-dev \
  libgoogle-glog-dev
