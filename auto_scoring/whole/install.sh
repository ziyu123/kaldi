#!/bin/bash

path=`pwd`


if [ "$1" == clean ]
then
	cd ${path}/pachira_score/src
	make $1

	cd engine
	make $1

	cd ${path}/pachira_score/mlp
	make $1

	cd ${path}/pachira_score/lib
	make $1
else
	. setenv.sh
	cd ${path}/pachira_score/src
	make 
	cp *.o ${path}/pachira_score/lib

	cd engine
	make 
	cp *.o ${path}/pachira_score/lib

	cd ${path}/pachira_score/mlp
	make 
	cp *.o ${path}/pachira_score/lib

	cd ${path}/pachira_score/lib
	make
fi
