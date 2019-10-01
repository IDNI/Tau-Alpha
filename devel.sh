#!/bin/sh

if [ -f ./submodules/emsdk/emsdk_env.sh ]; then
	source ./submodules/emsdk/emsdk_env.sh
fi

clear && ./debug.sh $@ && \
./build-Debug/alpha --docroot ./docroot --http-listen 0.0.0.0:6283
