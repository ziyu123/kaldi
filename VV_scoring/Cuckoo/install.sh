#!/bin/bash

path=`pwd`


if [ "$1" == clean ]
then
	make -C lib/src clean
	make -C lib/src/engine clean
	make -C lib/mlp clean
	make -C bin clean
else
	make -C lib/src
        make -C lib/src/engine
        make -C lib/mlp

	cp lib/*/*.o lib/*/*/*.o lib
	make -C lib 
	make -C bin
fi
