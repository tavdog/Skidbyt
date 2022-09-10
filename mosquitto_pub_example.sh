 # this only gifs can be pushed using this firmware
 #$MAC is displayed on the tidbyt after boot

# use this if you have the gif in base64 already
 mosquitto_pub -h $SERVER  -u $USER -p $PORT -P $PASS -t plm/$MAC/applet  -m '{"applet": "hello", "payload": "R0lGODlhQAAgAAAAACH5BAAFAAAALAAAAABAACAAgAAAAP///wJv8DF1ub2DiSnM+uSZcOdOM2X7mBH7IlKrLDMErsWNU22tSw+2bPhFariUMISTeTqzny2oAxYfvJ3kwqL2mKufdWfEfJ9g8ZhcNp/RafWa3Xa/4XH5nF633/F5/Z7f9/8BAwUHCQsNDxETFRcZvwoAADs="}' -r

# use this if you want to base64 encode the gif on the fly
 mosquitto_pub -h $SERVER  -u $USER -p $PORT -P $PASS -t plm/$MAC/applet -m '{"applet": "hello", "payload": "'$(base64 -w 0 -i noaa_buoy.gif)'"}' -r