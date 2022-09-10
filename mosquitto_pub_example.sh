 # this only gifs can be pushed using this firmware
 
 mosquitto_pub -h $SERVER  -u $USER -p $PORT -P $PASS -t plm/$MAC/applet  -m '{"applet": "hello", "payload": "R0lGODlhQAAgAAAAACH5BAAFAAAALAAAAABAACAAgAAAAP///wJv8DF1ub2DiSnM+uSZcOdOM2X7mBH7IlKrLDMErsWNU22tSw+2bPhFariUMISTeTqzny2oAxYfvJ3kwqL2mKufdWfEfJ9g8ZhcNp/RafWa3Xa/4XH5nF633/F5/Z7f9/8BAwUHCQsNDxETFRcZvwoAADs="}' -r