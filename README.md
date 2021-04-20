# panel-led-mqtt

Custom version of [panel-led-mqtt](https://github.com/domoticafacilconjota/panel-led-mqtt/tree/v0.100-RC) to be used with [Pixlet](https://github.com/cghdev/pixlet). This version reads base64 encoded GIFs via a MQTT topic and uses [GifDecoder library](https://github.com/pixelmatix/GifDecoder/) to render it in the LED panel.

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
