FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    pkg-config \
    clang \
    wget \
    tar \
    ninja-build

RUN wget https://github.com/Kitware/CMake/releases/download/v4.0.0-rc2/cmake-4.0.0-rc2-linux-x86_64.tar.gz && \
    tar -xzf cmake-4.0.0-rc2-linux-x86_64.tar.gz && \
    mv cmake-4.0.0-rc2-linux-x86_64 /opt/cmake && \
    ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake && \
    ln -s /opt/cmake/bin/ctest /usr/local/bin/ctest && \
    rm cmake-4.0.0-rc2-linux-x86_64.tar.gz

WORKDIR /app
