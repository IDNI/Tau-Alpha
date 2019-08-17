#!/bin/sh

if [ -f /emsdk/emsdk_env.sh ]; then
	source /emsdk/emsdk_env.sh
fi

clear && ./debug.sh $@ && ./build-Debug/alpha --docroot ./docroot --http-listen 127.0.0.1:6283
