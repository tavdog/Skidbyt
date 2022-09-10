# Tidbyt-mqtt

Custom version of [https://github.com/cghdev/panel-led-mqtt] with Tidbyt pins defined and ready to build. Also include binary (firmware.bin) to flash directly to the Tidbyt.

This firmware will only display gifs so make sure to pass --gif if you are using pixlet.

See mosquitto_pub_example.sh for an example of how to push via mqtt

# TOPIC MQTT

### plm/applet
#### Payload: JSON object containing applet name and base64 encoded GIF image
e.g:

`{"applet": "hello", "payload": "R0lGODlhQAAgAAAAACH5BAAFAAAALAAAAABAACAAgAAAAP///wJv8DF1ub2DiSnM+uSZcOdOM2X7mBH7IlKrLDMErsWNU22tSw+2bPhFariUMISTeTqzny2oAxYfvJ3kwqL2mKufdWfEfJ9g8ZhcNp/RafWa3Xa/4XH5nF633/F5/Z7f9/8BAwUHCQsNDxETFRcZvwoAADs="}`


### plm/brightness
#### Payload: Brightness level set as number, 0 to 100
e.g.:

`80`

### plm/status
"up" gets published when ESP32 boots up.


### plm/current
The current applet gets pushed when loaded

### To flash use esptool or espHome flash tool with the firmware.bin
esptool.py write_flash --flash_size detect 0x0 firmware.bin
