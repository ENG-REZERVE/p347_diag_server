/*
 * hwmon_comm_center.hpp
 *
 *  Created on: Oct 19, 2014
 *      Author: ukicompile
 */

#ifndef HWMON_COMM_CENTER_HPP_
#define HWMON_COMM_CENTER_HPP_

#include "rcf_bind_hwmon_client.hpp"
#include "hw_client_signaling.hpp"

typedef RCF::Publisher<PUB_hwmon> HWDATA_connection_publisher;
typedef boost::shared_ptr<HWDATA_connection_publisher> t_datacast_publisher_pointer;

class HwMonCommCenter : public UKI_LOG {
public:
	HwMonCommCenter(const char* ip, int port);
	~HwMonCommCenter();
	//-----------------------------------------------------
	hardware_monitor::ServerVersion getServerVersion();
	void setServerVersion(int major, int minor, std::string build);
	//-----------------------------------------------------
	int createHardwareMonitor(const hardware_monitor::HardwareMonitoringClientInitParams & hwmcip);
	int deleteHardwareMonitor();
	bool isHwMonitorCreated();
	//-----------------------------------------------------
	int changeLoggingParams(const hardware_monitor::HwmonLoggingParams & hwlp);
	int clientSubscribe(const hardware_monitor::SubscriptionMask & sm);
	int clientUnsubscribe();
	hardware_monitor::SubscriptionMask getCurrentSubscriptionMask();
	//------------------------------------------------------actions
	int doBeep(int msec);
	int doSuspend();
	int doShutdown();
	int doReboot();

	void onBatteryData(t_batmon_data* data, int error);
	void onCpuTemperatureData(float t_min, float t_max, int error);
	void onButtonData(unsigned char value, int error);

private:

	RCF::RcfServer*						server;
	p347_HwClientSignaling*				hwc;
	t_datacast_publisher_pointer 		pubPtr;

	hardware_monitor::SubscriptionMask	current_smask;
	hardware_monitor::ServerVersion 	sv;
	bool 								is_client_created;

	std::string							journal_ipc_path;
	int									journal_ipc_key;

	boost::signals2::connection			connection_battery;
	boost::signals2::connection			connection_temperature;
	boost::signals2::connection			connection_button;
};



#endif /* HWMON_COMM_CENTER_HPP_ */
