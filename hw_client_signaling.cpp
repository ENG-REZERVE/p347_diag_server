/*
 * hw_client_signaling.cpp
 *
 *  Created on: Oct 20, 2014
 *      Author: ukicompile
 */

#include "hw_client_signaling.hpp"

p347_HwClientSignaling::p347_HwClientSignaling() : p347_HwClient() {
	//do nothing
}

p347_HwClientSignaling::~p347_HwClientSignaling() {
	//do nothing
}

void p347_HwClientSignaling::callbackBatteryData(t_batmon_data* data, int error) {
	sigBatteryData(data,error);
}

void p347_HwClientSignaling::callbackCPUTemperature(float t_min, float t_max, int error) {
	sigCPUTemperature(t_min,t_max,error);
}

void p347_HwClientSignaling::callbackPBEvent(unsigned char value, int error) {
	sigPBEvent(value,error);
}
