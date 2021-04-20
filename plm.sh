#!/bin/bash

[[ -z "$1" ]] && exit 1
config=$(cat data/config.json)
server=$(echo "$config" | jq -r .mqtt_server)
username=$(echo "$config" | jq -r .mqtt_user)
password=$(echo "$config" | jq -r .mqtt_password)
pixlet_server=$(echo "$config" | jq -r .pixlet_server)

data="$2"
topic="$1"
[[ -z "$2" && "$topic" != "list" ]] && exit
if [ "$topic" == "applet" ];then
    echo "[+] Generating applet"
    data=$(curl -s "http://$pixlet_server:8080/applet/$2") # https://github.com/cghdev/pixlet
    [[ "$data" == "" ]] && echo "Applet does not exist" && exit 1
    payload="{\"applet\": \"$2\", \"payload\": \"$data\"}"
elif [ "$topic" == "list" ];then
    echo "[+] Available applets:"
    data=$(curl -s "http://$pixlet_server:8080/applets" | jq -r .[])
    echo "$data"
    exit 0
else
    payload=$data
fi
echo "[+] Publishing data to MQTT server"
mosquitto_pub -h $server  -u "$username" -P "$password" -t "plm/$topic"  -m "$payload" -r
