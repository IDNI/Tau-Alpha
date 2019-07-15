#!/bin/bash

if [ ! -f ./build-Release/alpha ]; then
	./release.sh
fi

$(sleep 2 && xdg-open http://localhost:6283)&
./build-Release/alpha --docroot ./docroot --http-listen localhost:6283
