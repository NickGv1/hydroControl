/*
 * logWifiCtrl.cpp
 *
 *  Created on: 29 Apr 2017
 *      Author: nick
 */
#include 	"logWifiCtrl.h"

logWifiCtrl::logWifiCtrl() {


	Serial.begin(115200);
	delay(1000);

	Serial.printf("\n\nStarting hydroCtrl..!\n");
	Serial.printf("Set DebugOuput to true\n");
	Serial.setDebugOutput(true);

	WiFiClient *client;

	client = new WiFiClient;

	Serial.printf("Initialising SD card\n");

	if (!SD.begin(4))
		Serial.printf("SD initialization failed!\n");

	Serial.printf("Wifi Status is [%d]\n",WiFi.status());

	ns_hydroControl::WifiReportStatus();

	// If the WiFi connection is not connected then connect it
	if (WiFi.status() != WL_CONNECTED)
	{

		Serial.printf("\n\nConnecting to [%s]\n",ns_hydroControl::WIFI_SSID.data());

		while(	WiFi.status() 			!= WL_CONNECTED &&
				ns_hydroControl::i_connectAttemptCount++ <= 10)
		{
			WiFi.begin(ns_hydroControl::WIFI_SSID.data(), ns_hydroControl::WIFI_P.data());
			delay(1000);
			Serial.printf(".");
	    }

	   	if(WiFi.status() != WL_CONNECTED)
	   	{
	   		ns_hydroControl::b_wifiLocal = true;
	   	    Serial.println("Failed to connect to "+String(ns_hydroControl::WIFI_SSID.data()));
	   	    Serial.println("Setting mode WIFI_STA ");
	   		WiFi.mode(WIFI_STA);
	   	}
	   	else
	   		ns_hydroControl::b_wifiLocal = false;

	   	Serial.printf("\nIP address: ");
	   	Serial.println(WiFi.localIP());

	    Serial.printf("\nIntialising ThingSpeak client");
	    ThingSpeak.begin(*client);

		ns_hydroControl::NTPTime *currentTime;
		currentTime = new ns_hydroControl::NTPTime();

	    Serial.printf("Retrieving the current NTP Time");

	    ns_hydroControl::b_status = currentTime->getNTPTime();

	}
}

logWifiCtrl::~logWifiCtrl() {
	// TODO Auto-generated destructor stub
}

