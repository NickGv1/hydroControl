/*
 * NTPTime.h
 *
 *  Created on: 30 Apr 2017
 *      Author: nick
 */


#ifndef NTPTIME_H_
#define NTPTIME_H_

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include <WiFiUdp.h>

namespace ns_hydroControl {


	extern const std::string		NTPServerName;
	extern const std::string		WIFI_SSID;
	extern const std::string		WIFI_P;
	extern const std::string		TS_WRITE_API_KEY;
	extern const unsigned short 	NTPPacketSize;
	extern const unsigned short 	UDPLocalPort;
	extern const unsigned short 	REPORT_INTERVAL;
	extern const unsigned short 	TS_CHANNEL_ID;
	extern int 						i_connectAttemptCount;
	extern float       				currentTemperature;
	extern unsigned long			lastConnectionTime;
	extern bool						b_status;
	extern bool						b_wifiLocal;



	class NTPTime {
	public:
		NTPTime();
		virtual ~NTPTime();

		bool getNTPTime();
		bool sendNTPPacket();

		byte		packetBuffer[48];	// Buffer to hold incoming and outgoing packets
		String		timeStamp;

		IPAddress 	NTPServerIP;
		WiFiUDP		UDP;									// A UDP instance to let us send and receive packets over UDP


	};

}
#endif /* NTPTIME_H_ */
