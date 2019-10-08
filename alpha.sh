#!/bin/bash

if [ ! -f ./build-Release/alpha ]; then
	./release.sh
fi

./build-Release/alpha --docroot ./docroot --http-listen 0.0.0.0:6283
