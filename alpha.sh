#!/bin/bash

DIR=$(pwd)
BASEDIR=$( cd "$(dirname "$0")" ; pwd -P )
TMLDIR="$BASEDIR/submodules/TML"

if [ ! -f $TMLDIR/src/libTML.so ]; then
	cd $TMLDIR/src
	./release.sh
fi

cd $BASEDIR/src

if [ ! -f ./alpha ]; then
	./release.sh
fi

$(sleep 2 && xdg-open http://localhost:19061)&
./alpha --docroot ../docroot --http-listen localhost:19061

cd $DIR
