#!/bin/bash

if [ ! -f ./build-Release/alpha ]; then
	if [ -f /esmdk/emsdk_env.sh ]; then
		source /emsdk/emsdk_env.sh
	fi
	./release.sh
fi

#$(sleep 2 && xdg-open http://localhost:6283 2>&1 >/dev/null)&
./build-Release/alpha --docroot ./docroot --http-listen 0.0.0.0:6283
