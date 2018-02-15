#!/bin/sh

docker run --rm -it -p 192.168.100.1:1883:1883 -p 9001:9001 -v /Users/alessandro/Development/Arduino/ticina/hub/local_dev/server/mosquitto:/mosquitto eclipse-mosquitto:1.4.12
# docker run --rm -it -p 1883:1883 -p 9001:9001 -v /Users/alessandro/Development/Arduino/ticina/hub/local_dev/server/mosquitto:/mosquitto eclipse-mosquitto:1.4.12
