#!/bin/sh

rm -f ./build-Debug/alpha && clear && ./debug.sh $@ && ./build-Debug/alpha --docroot ./docroot --http-listen 127.0.0.1:6283

