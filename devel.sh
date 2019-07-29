#!/bin/sh

clear && ./debug.sh $@ && ./build-Debug/alpha --docroot ./docroot --http-listen 127.0.0.1:6283

