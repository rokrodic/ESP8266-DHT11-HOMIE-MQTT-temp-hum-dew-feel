# ESP8266-DHT11-HOMIE-MQTT-temp-hum-dew-feel
ESP8266 module + DHT11 -> get temperature and humidity, calculate dew point,real feel. Post result onto MQQT broker.

How to setup Items in OPENHAB:

Group gMQQT "MQQT" <colorwheel>

Group gHomie "HOMIE" <colorwheel> (All, gMQQT)

Group gH1 "H: [%s]" <colorwheel> (gHomie)


// "67490e0"

String gH1a0 "FW Name [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$fwname:state:default]"}

String gH1a1 "Name [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$name:state:default]"}

String gH1a2 "Local IP [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$localip:state:default]"}

String gH1a3 "Signal [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$signal:state:default]"}

String gH1a4 "Uptime [%ss]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$uptime:state:default]"}

Number gH1a5 "Temp [%.1f°C]"	<colorwheel> (gH1, gInfo)	{mqtt="<[mosquitto:devices/67490e0/temperature/degrees:state:default]"}

Number gH1a6 "Humidity [%s%%]"	<colorwheel> (gH1, gInfo)	{mqtt="<[mosquitto:devices/67490e0/humidity/percent:state:default]"}

Number gH1a7 "Heat index [%.1f°C]"	<colorwheel> (gH1)	{mqtt="<[mosquitto:devices/67490e0/heatindex/degrees:state:default]"}

Number gH1a8 "Dew point [%s°C]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/dew/degrees:state:default]"}

String gH1a9 "Unit [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/temperature/unit:state:default]"}

String gH1a10 "FW Ver [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$fwversion:state:default]"}

String gH1a11 "Online [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$online:state:default]"}

String gH1a12 "OTA [%s]"	<colorwheel> (gH1)		{mqtt="<[mosquitto:devices/67490e0/$ota/enabled:state:default]"}

