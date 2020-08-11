#!/usr/bin/env bash

mkdir -p ./src
cp -R ../xbee_ansic_library/src/* ./src
cp -R ../xbee_ansic_library/include/* ./src

mkdir -p ./src/ports
cp -R ../xbee_ansic_library/ports/* ./src/ports
