#!/bin/bash

if [[ $1 == clean ]]; then
	make -C peak/ clean
	make -C score/ clean
	make -C gmm/ clean
else
	make -C peak/
	make -C score/
	make -C gmm/
fi
