/*
 * rcf_bind_channel_manager.hpp
 *
 *  Created on: Sep 21, 2014
 *      Author: ukicompile
 */

#ifndef RCF_BIND_HWMON_CLIENT_HPP_
#define RCF_BIND_HWMON_CLIENT_HPP_

#include <RCF/RCF.hpp>
#include "hardware_monitor.pb.h"

RCF_BEGIN(HWMON_connection,"HWMON_connection")
	RCF_METHOD_R0(hardware_monitor::ServerVersion,getServerVersion)
	//-----------------------------------------------------
	RCF_METHOD_R1(int, createHardwareMonitor, const hardware_monitor::HardwareMonitoringClientInitParams &)
	RCF_METHOD_R0(int, deleteHardwareMonitor)
	RCF_METHOD_R0(bool, isHwMonitorCreated)
	//-----------------------------------------------------
	RCF_METHOD_R1(int, changeLoggingParams, const hardware_monitor::HwmonLoggingParams &)
	RCF_METHOD_R1(int, clientSubscribe, const hardware_monitor::SubscriptionMask &)
	RCF_METHOD_R0(int, clientUnsubscribe)
	RCF_METHOD_R0(hardware_monitor::SubscriptionMask, getCurrentSubscriptionMask)
	//------------------------------------------------------actions
	RCF_METHOD_R1(int, doBeep, int)
	RCF_METHOD_R0(int, doSuspend)
	RCF_METHOD_R0(int, doShutdown)
	RCF_METHOD_R0(int, doReboot)
RCF_END(HWMON_connection)

RCF_BEGIN(PUB_hwmon,"PUB_hwmon")
	//---Publish
	//RCF_METHOD_V1(void, alarmError, int)
	RCF_METHOD_V1(void, batteryData, const hardware_monitor::BatteryInformation &)
	RCF_METHOD_V1(void, cpuTemperatureData, const hardware_monitor::TemperatureInformation &)
	RCF_METHOD_V1(void, buttonData, const hardware_monitor::ButtonInformation &)
RCF_END(PUB_connection)

#endif /* RCF_BIND_CHANNEL_MANAGER_HPP_ */
