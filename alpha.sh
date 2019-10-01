#!/bin/bash

if [ -f ./submodules/emsdk/emsdk_env.sh ]; then
	source ./submodules/emsdk/emsdk_env.sh
fi

if [ ! -f ./build-Release/alpha ]; then
	./release.sh
fi

#$(sleep 2 && xdg-open http://localhost:6283 2>&1 >/dev/null)&
./build-Release/alpha --docroot ./docroot --http-listen 0.0.0.0:6283
