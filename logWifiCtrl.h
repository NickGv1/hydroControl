/*
 * logWifiCtrl.h
 *
 *  Created on: 29 Apr 2017
 *      Author: nick
 */

#ifndef LOGWIFICTRL_H_
#define LOGWIFICTRL_H_

#include "Arduino.h"
#include "ThingSpeak.h"
#include "SD.h"
#include <WiFiClient.h>
#include "NTPTime.h"


class logWifiCtrl {
public:
	logWifiCtrl();
	virtual ~logWifiCtrl();

};
#endif /* LOGWIFICTRL_H_ */
