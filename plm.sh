#!/bin/bash

[[ -z "$1" ]] && exit 1
config=$(cat data/config.json)
server=$(echo "$config" | jq -r .mqtt_server)
username=$(echo "$config" | jq -r .mqtt_user)
password=$(echo "$config" | jq -r .mqtt_password)

data="$2"
topic="$1"
[[ "$topic" == "applet" ]] && data=$(curl -s "http://192.168.1.10:8080/applet/$2") # https://github.com/cghdev/pixlet

docker run --rm -it ruimarinho/mosquitto mosquitto_pub -h $server  -u "$username" -P "$password" -t "plm/$topic"  -m "$data"
