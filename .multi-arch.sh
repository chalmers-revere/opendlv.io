#!/bin/sh

for microservice in \
    proxy-oxts-latest proxy-oxts-v0.0.1 \
    ; do
cat <<EOF >/tmp/multi.yml
image: chalmersrevere/opendlv.io-multi:$microservice
manifests:
  - image: chalmersrevere/opendlv.io-amd64:$microservice
    platform:
      architecture: amd64
      os: linux
  - image: chalmersrevere/opendlv.io-armhf:$microservice
    platform:
      architecture: arm
      os: linux
  - image: chalmersrevere/opendlv.io-aarch64:$microservice
    platform:
      architecture: arm64
      os: linux
EOF
manifest-tool-linux-amd64 push from-spec /tmp/multi.yml
done
