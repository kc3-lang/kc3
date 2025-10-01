FROM gradle:6.9.4-jdk11

USER root

# Install autotools and other build dependencies
RUN apt-get update && apt-get install -y \
    autoconf \
    automake \
    libtool \
    texinfo \
    && rm -rf /var/lib/apt/lists/*

USER gradle
