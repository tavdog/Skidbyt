#!/bin/bash

[[ -z "$1" ]] && exit 1
config=$(cat data/config.json)
server=$(echo "$config" | jq -r .mqtt_server)
username=$(echo "$config" | jq -r .mqtt_user)
password=$(echo "$config" | jq -r .mqtt_password)
pixlet_server=$(echo "$config" | jq -r .pixlet_server)

data="$2"
topic="$1"
if [[ "$topic" == "applet" ]];then
    data=$(curl -s "http://$pixlet_server:8080/applet/$2") # https://github.com/cghdev/pixlet
    [[ "$data" == "" ]] && echo "Applet does not exist" && exit 1
    payload="{\"applet\": \"$2\", \"payload\": \"$data\"}"
else
    payload=$data
fi
mosquitto_pub -h $server  -u "$username" -P "$password" -t "plm/$topic"  -m "$payload" -r
