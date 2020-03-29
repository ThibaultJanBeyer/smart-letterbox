#!/bin/bash

docker run -d -p 1883:1883 -p 9001:9001 -v ${PWD}/data:/mosquitto/data -v ${PWD}/log:/mosquitto/log eclipse-mosquitto -name
