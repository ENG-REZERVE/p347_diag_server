#include "hwmon_comm_center.hpp"
#include "protobuf_convertors.h"
#include "p347_ipc_common.h"

HwMonCommCenter::HwMonCommCenter(const char* ip, int port) {
	setPrefix("HwCommCenter");
	setConsoleLevel(LOG_LEVEL_FULL);

	is_client_created = false;
	hwc = NULL;

	if (RCF::getInitRefCount() <= 0)
		RCF::init();

	server = new RCF::RcfServer(RCF::TcpEndpoint(ip,port));
	assert(server != NULL);

	server->bind<HWMON_connection>(*this);

	server->start();

	pubPtr = server->createPublisher<PUB_hwmon>();

	logMessage(LOG_LEVEL_FULL,"HWmon comm start complete!\n");
}

HwMonCommCenter::~HwMonCommCenter() {
	logMessage(LOG_LEVEL_FULL,"deleting...\n");
	pubPtr->close();
	server->stop();
	deleteHardwareMonitor();

	delete server;
	logMessage(LOG_LEVEL_FULL,"destructor complete!\n");
}

//-----------------------------------------------------
hardware_monitor::ServerVersion HwMonCommCenter::getServerVersion() {
	hardware_monitor::ServerVersion sv_ret;

	sv_ret.CopyFrom(sv);
	printf("getServerVersion: %d.%d b%s",sv_ret.major(),sv_ret.minor(),sv_ret.build().c_str());

	return sv_ret;
}

void HwMonCommCenter::setServerVersion(int major, int minor, std::string build) {
	sv.set_error_code(0);
	sv.set_major(major);
	sv.set_minor(minor);
	sv.set_build(build);
}

//-----------------------------------------------------
int HwMonCommCenter::createHardwareMonitor(const hardware_monitor::HardwareMonitoringClientInitParams & hwmcip) {
	if (is_client_created) return -EBUSY;

	//----------------------------------------------------------step1
	hwc = new p347_HwClientSignaling();
	if (hwc == NULL) return -ENOMEM;

	if (hwmcip.has_journal_ipc_key()) journal_ipc_key = hwmcip.journal_ipc_key();
	else journal_ipc_key = 45;

	if (hwmcip.has_journal_ipc_path()) journal_ipc_path = hwmcip.journal_ipc_path();
	else journal_ipc_path = "/mnt/share";

	//----------------------------------------------------------step2
	//TODO: move default values to ini
	int own_ipc_key = 22;
	if (hwmcip.has_own_ipc_key()) own_ipc_key = hwmcip.own_ipc_key();

	std::string own_ipc_path = "/etc";
	if (hwmcip.has_own_ipc_path()) own_ipc_path = hwmcip.own_ipc_path();

	int ret = hwc->initQueue(own_ipc_path.c_str(),own_ipc_key);
	if (ret != 0) {
		logMessage(LOG_LEVEL_CRITICAL,"createHardwareMonitor failed at initQueue with %d\n",ret);
		delete hwc;
		hwc = NULL;
		return ret;
	}

	//----------------------------------------------------------step3

	int hwserver_ipc_key = 44;
	if (hwmcip.has_hwserver_ipc_key()) hwserver_ipc_key = hwmcip.hwserver_ipc_key();

	std::string hwserver_ipc_path = "/mnt/share";
	if (hwmcip.has_hwserver_ipc_path()) hwserver_ipc_path = hwmcip.hwserver_ipc_path();

	ret = hwc->initIPCServerConnection(hwserver_ipc_path.c_str(),hwserver_ipc_key,hwmcip.srv_conn_wait_msec(),hwmcip.srv_conn_retry_cnt());
	if (ret != 0) {
		hwc->deleteQueue();

		logMessage(LOG_LEVEL_CRITICAL,"createHardwareMonitor failed at initIPCServerConnection with %d\n",ret);
		delete hwc;
		hwc = NULL;
		return ret;
	}

	connection_battery = hwc->sigBatteryData.connect(boost::bind(&HwMonCommCenter::onBatteryData,this,_1,_2));
	connection_temperature = hwc->sigCPUTemperature.connect(boost::bind(&HwMonCommCenter::onCpuTemperatureData,this,_1,_2,_3));
	connection_button = hwc->sigPBEvent.connect(boost::bind(&HwMonCommCenter::onButtonData,this,_1,_2));

	is_client_created = true;
	return 0;
}

int HwMonCommCenter::deleteHardwareMonitor() {


	if (hwc != NULL) {
		connection_battery.disconnect();
		connection_temperature.disconnect();
		connection_button.disconnect();

		hwc->deleteQueue();

		delete hwc;
		hwc = NULL;
	}

	is_client_created = false;

	return 0;
}

bool HwMonCommCenter::isHwMonitorCreated() {
	return is_client_created;
}

//-----------------------------------------------------
int HwMonCommCenter::changeLoggingParams(const hardware_monitor::HwmonLoggingParams & hwlp) {
	setConsoleLevel(hwlp.main_console_level());
	setFileLevel(hwlp.main_console_level(),true,NULL);
	setDaemonLevel(hwlp.main_console_level(),journal_ipc_path.c_str(),journal_ipc_key);

	if (hwc == NULL) return -ENOENT;

	hwc->setConsoleLevel(hwlp.hwclient_console_level());
	hwc->setFileLevel(hwlp.hwclient_console_level(),true,NULL);
	hwc->setDaemonLevel(hwlp.hwclient_console_level(),journal_ipc_path.c_str(),journal_ipc_key);

	return 0;
}

int HwMonCommCenter::clientSubscribe(const hardware_monitor::SubscriptionMask & sm) {
	if (hwc == NULL) return -ENOENT;

	unsigned short event_mask = 0;
	if (sm.has_battery()) event_mask |= SUB_MASK_BATTERY_STATE;
	if (sm.has_cpu_temperature()) event_mask |= SUB_MASK_CPU_TEMPERATURE;
	if (sm.has_pbutton_info()) event_mask |= SUB_MASK_BUTTON_VIEW;
	if (sm.has_pbutton_action()) event_mask |= SUB_MASK_BUTTON_ACTION;

	if (event_mask == 0) return -EINVAL;

	int ret = hwc->subscribeToEvents(event_mask);
	if (ret != 0) {
		logMessage(LOG_LEVEL_CRITICAL,"subscribeToEvents failed with %d\n", ret);
		return ret;
	}

	current_smask.CopyFrom(sm);
	hwc->Resume();

	return 0;
}

int HwMonCommCenter::clientUnsubscribe() {
	if (hwc == NULL) return -ENOENT;

	int ret = hwc->unsubscribe();

	if (ret != 0) {
		logMessage(LOG_LEVEL_CRITICAL,"unsubscribe failed with %d\n", ret);
		return ret;
	}

	current_smask.set_error_code(0);
	current_smask.set_battery(false);
	current_smask.set_cpu_temperature(false);
	current_smask.set_pbutton_info(false);
	current_smask.set_pbutton_action(false);

	return 0;
}

hardware_monitor::SubscriptionMask HwMonCommCenter::getCurrentSubscriptionMask() {
	hardware_monitor::SubscriptionMask		sm_ret;

	if (hwc == NULL) {
		sm_ret.set_error_code(-ENOENT);
		return sm_ret;
	}

	sm_ret.CopyFrom(current_smask);

	return sm_ret;
}

//------------------------------------------------------actions
int HwMonCommCenter::doBeep(int msec) {
	if (hwc == NULL) return -ENOENT;
	return hwc->doBeep(msec);
}

int HwMonCommCenter::doSuspend() {
	if (hwc == NULL) return -ENOENT;
	return hwc->doSuspend();
}

int HwMonCommCenter::doShutdown() {
	if (hwc == NULL) return -ENOENT;
	return hwc->doShutdown();
}

int HwMonCommCenter::doReboot() {
	if (hwc == NULL) return -ENOENT;
	return hwc->doReboot();
}

//--------------------------------------------------------casters
//void HwMonCommCenter::_alarmError(int error_code) {
//	pubPtr->publish().alarmError(error_code);
//}

void HwMonCommCenter::onBatteryData(t_batmon_data* data, int error) {
	hardware_monitor::BatteryInformation bi;
	if (data == NULL) {
		bi.set_error_code(-EINVAL);
	} else {
		int ret = packBatteryInformation(&bi, data);
		if (ret != 0) {
			bi.set_error_code(ret);
		} else {
			bi.set_error_code(error);
		}
	}

	pubPtr->publish().batteryData(bi);
}

void HwMonCommCenter::onCpuTemperatureData(float t_min, float t_max, int error) {
	hardware_monitor::TemperatureInformation ti;

	ti.set_error_code(error);
	ti.set_t_min(t_min);
	ti.set_t_max(t_max);

	pubPtr->publish().cpuTemperatureData(ti);
}

void HwMonCommCenter::onButtonData(unsigned char value, int error) {
	hardware_monitor::ButtonInformation bi;

	bi.set_error_code(error);
	bi.set_is_pressed(value != 0);

	pubPtr->publish().buttonData(bi);
}
