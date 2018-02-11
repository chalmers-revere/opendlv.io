#!/bin/sh

URL=$1
VERSION=$2

#curl --data build=true -X POST $URL
curl -H "Content-Type: application/json" --data '{"source_type": "Tag", "source_name": "${VERSION}"}' -X POST ${URL}

