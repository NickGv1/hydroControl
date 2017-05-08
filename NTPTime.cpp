/*
 * NTPTime.cpp
 *
 *  Created on: 30 Apr 2017
 *      Author: nick
 */

#include "NTPTime.h"

namespace ns_hydroControl {

	const std::string 		NTPServerName			= "pool.ntp.org";		// Use the pool NTP Sever to find a local server
	const std::string 		WIFI_SSID				= "SKY97C46";
	const std::string 		WIFI_P					= "YFCDVQFCNY";
	const std::string 		TS_WRITE_API_KEY		= "8E99W4U5C3I7DCS8";  // Write API key for Wifi Temp ThingsSpeak Channel
	const unsigned short 	UDPLocalPort 			= 2390;
	int						i_connectAttemptCount 	= 0;
	bool					b_status				= false;
	bool					b_wifiLocal				= false;


	//		const unsigned short 	NTPPacketSize			= 48;
	//		const unsigned short 	REPORT_INTERVAL			= 900;     			// Publish data every 15 minutes
	//		const unsigned short 	TS_CHANNEL_ID			= 54452;               // Channel ID for Wifi Temp ThingsSpeak Channel
	//		float					currentTemperature  	= -1.0; 			// The current temperature reading<YOUR-CHANNEL-WRITEAPIKEY>"
	//		unsigned long			lastConnectionTime  	= 0;    			// track the last connection time

	void WifiReportStatus() {

	  switch(WiFi.status()) {
	    case  WL_CONNECTED:
	      b_status = Serial.println("Wifi.Status: WL_CONNECTED: assigned when connected to a WiFi network");
	      break;
	    case  WL_NO_SHIELD:
	    	b_status = Serial.println("Wifi.Status: WL_NO_SHIELD: assigned when no WiFi shield is present");
	      break;
	    case  WL_IDLE_STATUS:
	    	b_status = Serial.println("WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() \nis called and remains active until the number of attempts expires \n(resulting in WL_CONNECT_FAILED) or a connection is established \n(resulting in WL_CONNECTED);");
	      break;
	    case  WL_NO_SSID_AVAIL:
	    	b_status = Serial.println("Wifi.Status: WL_NO_SSID_AVAIL: assigned when no SSID are available");
	      break;
	    case  WL_SCAN_COMPLETED:
	    	b_status = Serial.println("Wifi.Status: WL_SCAN_COMPLETED: assigned when the scan networks is completed");
	      break;
	    case  WL_CONNECT_FAILED:
	    	b_status = Serial.println("Wifi.Status: WL_CONNECT_FAILED: assigned when the connection fails for all the attempts");
	      break;
	    case  WL_CONNECTION_LOST:
	    	b_status = Serial.println("Wifi.Status: WL_CONNECTION_LOST: assigned when the connection is lost;");
	      break;
	    case  WL_DISCONNECTED:
	    	b_status = Serial.println("Wifi.Status: WL_DISCONNECTED: assigned when disconnected from a network;");
	      break;
	  }

	}


	NTPTime::NTPTime() {

		// Open a local port to retrieve ntp time over udp
		Serial.printf("Starting UDP\n");
		UDP.begin(UDPLocalPort);

		//get a random server from the pool
		Serial.printf("Resolving NTP server address ");
		WiFi.hostByName(NTPServerName.data(), NTPTime::NTPServerIP);
		Serial.printf("[");
		Serial.print(NTPTime::NTPServerIP);
		Serial.printf("]\n");

		// Retrieve the current NTP time
		b_status = NTPTime::getNTPTime();

		if (b_status) {
			Serial.printf("The current NTP time is [");
			Serial.print(NTPTime::timeStamp);
			Serial.printf("]\n");
		} else
			Serial.printf("Failed to retrieve NTP time\n");
	};

	NTPTime::~NTPTime() {
	};

	bool NTPTime::getNTPTime() {

		int i_ReadCount=0;

		b_status = NTPTime::sendNTPPacket();

		// wait to see if a reply is available
		delay(1000);

		b_status = UDP.parsePacket();

		if (!b_status)
			Serial.printf("Waiting for udp packet ");

		while (!b_status) {

			if (i_ReadCount++ > 9) // On the Tenth Attempt then fail and restart
			{
				Serial.printf("Failed to retrieve udp packet. Re-requesting NTP Time\n");
				UDP.flush();
				UDP.stop();
				delay(1000);
				UDP.begin(UDPLocalPort);
				b_status = NTPTime::getNTPTime();
				return false;
			}

			Serial.printf(".");
			ns_hydroControl::b_status = UDP.parsePacket();
			delay(5000);
		}

		Serial.println("");

		// We've received a packet, read the dans_hydroControl::NTPTime::timeStamp = ns_hydroControl::NTPTime::timeStamp + String(epoch % 60); // print the secondta from it
		UDP.read(NTPTime::packetBuffer, 48); // read the packet into the buffer

		//the timestamp starts at byte 40 of the received packet and is four bytes,
		// or two words, long. First, esxtract the two words:

		unsigned long highWord = word(NTPTime::packetBuffer[40],NTPTime::packetBuffer[41]);
		unsigned long lowWord = word(NTPTime::packetBuffer[42],NTPTime::packetBuffer[43]);

		// combine the four bytes (two words) into a long integer
		// this is NTP time (seconds since Jan 1 1900):
		unsigned long secsSince1900 = highWord << 16 | lowWord;

		// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
		const unsigned long seventyYears = 2208988800UL;
		// subtract seventy years:
		unsigned long epoch = secsSince1900 - seventyYears;

		// print the hour, minute and second:
		NTPTime::timeStamp = String((epoch % 86400L) / 3600) + ":";

		if (((epoch % 3600) / 60) < 10) // In the first 10 minutes of each hour, we'll want a leading '0'
			NTPTime::timeStamp = NTPTime::timeStamp + "0";

		NTPTime::timeStamp = NTPTime::timeStamp + String((epoch % 3600) / 60) + ":"; // print the minute (3600 equals secs per minute)

		if (epoch % 60 < 10) // In the first 10 seconds of each minute, we'll want a leading '0'
			NTPTime::timeStamp = NTPTime::timeStamp + "0";

		NTPTime::timeStamp = NTPTime::timeStamp + String(epoch % 60); // print the second

		return true;
	}

	bool NTPTime::sendNTPPacket() {

		// set all bytes in the buffer to 0
		memset(NTPTime::packetBuffer, 0, 48);

		// Initialize values needed to form NTP request
		// (see URL above for details on the packets)
		NTPTime::packetBuffer[0] = 0b11100011;   // LI, Version, Mode
		NTPTime::packetBuffer[1] = 0;     // Stratum, or type of clock
		NTPTime::packetBuffer[2] = 6;     // Polling Interval
		NTPTime::packetBuffer[3] = 0xEC;  // Peer Clock Precision

		// 8 bytes of zero for Root Delay & Root Dispersion
		NTPTime::packetBuffer[12] = 49;
		NTPTime::packetBuffer[13] = 0x4E;
		NTPTime::packetBuffer[14] = 49;
		NTPTime::packetBuffer[15] = 52;

		// all NTP fields have been given values, now
		// you can send a packet requesting a timestamp:
		UDP.beginPacket(NTPTime::NTPServerIP, 123); //NTP requests are to port 123
		UDP.write(NTPTime::packetBuffer, 48);
		UDP.endPacket();

		return true;

	};

}
