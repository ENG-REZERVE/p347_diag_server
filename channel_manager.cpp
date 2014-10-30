#include "channel_manager.hpp"

#include <sys/stat.h>
#include <sys/syscall.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace channel_manager;

ChannelManager::ChannelManager() {
	setPrefix("ChanMan");
}

ChannelManager::~ChannelManager() {
	exitManager();
}

int ChannelManager::initManager(channel_manager::ChannelManagerInitParams par) {
	//if (par == NULL) return -EINVAL;

	setFileLevel(par.log_level_file(),true,NULL);
	setConsoleLevel(par.log_level_console());
	setDaemonLevel(par.log_level_daemon(),par.daemon_ipc_path().c_str(),par.daemon_ipc_key());

	setBaseTiming(par.base_timing());

	int ret = openDevice(par.spi_speed_hz());
	if (ret != 0) {
		logMessage(LOG_LEVEL_CRITICAL,"Cannot open fpga device! ret=%d\n",ret);
	} else {
		if (par.reset_at_open()) {
			ret = resetFPGA();
			if (ret != 0) {
				logMessage(LOG_LEVEL_CRITICAL,"FPGA not configured after reset with err=%d\n",ret);
			}
		}
		if (ret == 0) {
			createChannels(par.main_sleep_us(),par.idle_sleep_us());
			//TODO: check for chan_level_file_size() etc
			for (int i=0; i<p347_ADC_CHANNELS_CNT; i++) {
				adcc[i]->setFileLevel(par.chan_level_file(i),true,NULL);
				adcc[i]->setConsoleLevel(par.chan_level_console(i));
				adcc[i]->setDaemonLevel(par.chan_level_daemon(i),par.daemon_ipc_path().c_str(),par.daemon_ipc_key());
			}

			dsph->setPrefix("DspHelper");
			dsph->setFileLevel(par.dsph_level_file(),true,NULL);
			dsph->setConsoleLevel(par.dsph_level_console());
			dsph->setDaemonLevel(par.dsph_level_daemon(),par.daemon_ipc_path().c_str(),par.daemon_ipc_key());

			t_driver_timings tdt;
			//load tdt from protoclass
			//const channel_manager::ChannelManagerInitParams_DriverTimings *cmit_dt = &par.driver_timings();
			//channel_manager::ChannelManagerInitParams_DriverTimings* cmit_dt = par.mutable_driver_timings();

			channel_manager::DriverTimings* cmit_dt = par.mutable_driver_timings();

			tdt.adc_run = cmit_dt->adc_run();
			tdt.adc_run_sync = cmit_dt->adc_run_sync();
			tdt.adc_set_params1 = cmit_dt->adc_set_params1();
			tdt.adc_set_params2 = cmit_dt->adc_set_params2();
			tdt.adc_set_params3 = cmit_dt->adc_set_params3();
			tdt.rot_run = cmit_dt->rot_run();

			par.release_driver_timings();

			setDriverTimings(&tdt);

			logMessage(LOG_LEVEL_FULL,"initManager finished with %d\n",ret);
		}

	}

	return ret;
}

void ChannelManager::exitManager() {
	deleteChannels();
	closeDevice();
}

int ChannelManager::FPGADriverReload(const std::string old_name, const std::string new_path) {
	//if (new_path == NULL) return -EINVAL;
	int ret = 0;
    unsigned long len = 0;
    int fd;
    void *buff = NULL;
    struct stat module_stat;

    //first prepare all new data before removing old
    ret = stat(new_path.c_str(),&module_stat);
    if (ret != 0) {
    	ret = -errno;
    	logMessage(LOG_LEVEL_CRITICAL,"PGADriverReload: new file stat = %d\n",ret);
    	return ret;
    }

    buff = malloc(module_stat.st_size);
	if (buff == NULL) {
		logMessage(LOG_LEVEL_CRITICAL,"PGADriverReload: no memory for buff\n");
		return -ENOMEM;
    }

    fd = open(new_path.c_str(),O_RDONLY);
    if (fd < 0) {
    	logMessage(LOG_LEVEL_CRITICAL,"PGADriverReload: cannot open %s\n",new_path.c_str());
    	return -errno;
    }

    if (module_stat.st_size != read(fd,buff,module_stat.st_size)) {
        logMessage(LOG_LEVEL_CRITICAL,"failed to read new module\n");
    	ret = -errno;
    } else {

		//remove old
		if (old_name.c_str() != "") {
			ret = syscall(__NR_delete_module,old_name.c_str(),O_TRUNC);
			if (ret < 0) {
				ret = -errno;
				logMessage(LOG_LEVEL_CRITICAL,"module_remove ret=%D\n",ret);
			} else {
				logMessage(LOG_LEVEL_MAIN,"module_remove = %d\n",ret);
			}
		}

		//insert new
    	ret = syscall(__NR_init_module, buff, module_stat.st_size, NULL);
    }

    free(buff);
    close(fd);

	return ret;
}

bool ChannelManager::isAnyADCChannelRunning(void) {
	bool ret = false;

	for (int i=0; i<p347_ADC_CHANNELS_CNT; i++)
		if (adcc[i]->isRunning()) {
			ret = true;
			break;
		}

	return ret;
}

bool ChannelManager::isAnyRotChannelRunning(void) {
	bool ret = false;

	for (int i=0; i<p347_ROT_CHANNELS_CNT; i++)
		if (isRotRunning(i)) {
			ret = true;
			break;
		}

	return ret;
}

void ChannelManager::alarmError(int error_code) {
	//TODO
	//send error from devHelper to high levels
	sigAlarm(error_code);
}

void ChannelManager::serverPing(int ping_code) {
	logMessage(LOG_LEVEL_MAIN,"Channel manager ping %d\n",ping_code);
}

//-------------------------------------------------------------------------DevHelper wrappers

channel_manager::ADCTimeOffsets ChannelManager::readADCTimeOffsets(void) {
	unsigned long off_data[p347_ADC_CHANNELS_CNT];
	memset(&off_data[0],0,p347_ADC_CHANNELS_CNT*sizeof(unsigned long));

	int ret = readOffsets(&off_data[0]);

	channel_manager::ADCTimeOffsets send_off;
	send_off.set_error_code(ret);

	if (ret == 0) {
		for (int i=0; i<p347_ADC_CHANNELS_CNT; i++) {
			send_off.add_offset(off_data[i]);
		}
	}

	return send_off;
}

static int avr_test = 0;

channel_manager::RotData ChannelManager::readRotData(unsigned char rot_idx) {
	t_rot_data rot_data;
	memset(&rot_data,0,sizeof(t_rot_data));

	int ret = getFreq(rot_idx, &rot_data);
	logMessage(LOG_LEVEL_FULL,"getFreq(%d) ret %d\n",rot_idx,ret);

	channel_manager::RotData send_rot;
	send_rot.set_error_code(ret);

	if (ret == 0) {
		send_rot.set_cnt(rot_data.cnt);
		//test!
		//		avr_test++;
		//		send_rot.set_freq_avr(avr_test);
		//		printf("set avr = %.5lf\n",send_rot.freq_avr());
		send_rot.set_freq_avr(rot_data.freq_avr);
		send_rot.set_freq_max(rot_data.freq_max);
		send_rot.set_freq_min(rot_data.freq_min);
		send_rot.set_overhigh(rot_data.overhigh);
		send_rot.set_avr(rot_data.avr);
		send_rot.set_max(rot_data.max);
		send_rot.set_min(rot_data.min);
		send_rot.set_pos(rot_data.pos);
		send_rot.set_running(rot_data.running);
		send_rot.set_sum(rot_data.sum);
		send_rot.set_underlow(rot_data.overhigh);
	}

	return send_rot;
}

int ChannelManager::doStartRotChannel(channel_manager::RotChannelInitParams & input_rp) {
	int invalid_params = 0;
	t_rot_channel_init_params rot_params;
	memset(&rot_params,0,sizeof(t_rot_channel_init_params));

	//printClass(&input_rp,TRS_CHANNEL_MANAGER_INIT_PARAMS);

	invalid_params = extractStructureFromClass(&rot_params,&input_rp,TRS_CHANNEL_MANAGER_INIT_PARAMS);
	if (invalid_params < 0) {
		logMessage(LOG_LEVEL_CRITICAL,"Failed to convert RotChannelInitParams protoclass to t_rot_channel_init_params structure with %d\n",invalid_params);
		return invalid_params;
	}

	//printStructure(&rot_params,TRS_CHANNEL_MANAGER_INIT_PARAMS);

	int ret = startRotChannel(&rot_params);

	return ret;
}

int ChannelManager::doSetupAdcChannel(channel_manager::ADCChannelInitParams & input_ap) {
	bool invalid_params = false;
	t_adc_channel_init_params aci_params;
	memset(&aci_params,0,sizeof(t_adc_channel_init_params));

	//TODO: make someday auto-extraction for nested structures
	if (input_ap.has_ch_idx())			aci_params.ch_idx = input_ap.ch_idx();
	if (input_ap.has_rot_idx())			aci_params.rot_idx = input_ap.rot_idx();
	if (input_ap.has_drv_buf_size())	aci_params.drv_buf_size = input_ap.drv_buf_size();
	if (input_ap.has_usr_proc_len())	aci_params.usr_proc_len = input_ap.usr_proc_len();
	if (input_ap.has_usr_proc_cnt())	aci_params.usr_proc_cnt = input_ap.usr_proc_cnt();
	if (input_ap.has_sen_filter_id())	aci_params.sen_filter_id = input_ap.sen_filter_id();
	//nested structure
	if (input_ap.has_ap()) {
		channel_manager::ADCParams* adcp = input_ap.mutable_ap();
		if (adcp->has_control1())		aci_params.ap.control1 	= adcp->control1();
		if (adcp->has_control2())		aci_params.ap.control2 	= adcp->control2();
		if (adcp->has_offset())			aci_params.ap.offset 	= adcp->offset();
		if (adcp->has_gain())			aci_params.ap.gain		= adcp->gain();
		if (adcp->has_overrange())		aci_params.ap.overrange	= adcp->overrange();
		if (adcp->has_ch_settings())	aci_params.ap.ch_settings 	= adcp->ch_settings();
	}

	if (invalid_params) {
		logMessage(LOG_LEVEL_CRITICAL,"Incoming ADCChannelInitParams isn't contains some of required fields\n");
		return -EINVAL;
	}

	int ret = setupAdcChannel(&aci_params);

	return ret;
}

int ChannelManager::doStartAdcChannel(unsigned char ch_idx) {
	if ((ch_idx < 1) || (ch_idx > p347_ADC_CHANNELS_CNT)) {
		logMessage(LOG_LEVEL_CRITICAL,"Invalid channel index (%d) in doStartAdcChannel\n",ch_idx);
		return -EINVAL;
	}
	if (adcc[ch_idx-1]->isRunning()) {
		logMessage(LOG_LEVEL_CRITICAL,"Channel %d is already running, cannot start!\n",ch_idx);
		return -EBUSY;
	}
	if (!adcc[ch_idx-1]->isSetuped()) {
		logMessage(LOG_LEVEL_CRITICAL,"Channel %d is not setuped, cannot start!\n",ch_idx);
		return -p347_ERROR_SYSTEM_CHANNEL_NOT_CONFIGURED;
	}

	adcc[ch_idx-1]->prepareCopy();
	adcc[ch_idx-1]->prepareRetranslate();

	return startAdcChannel(ch_idx);
}

int ChannelManager::doStartSyncChannels(const channel_manager::SynctaskChannels & input_tsc) {
	bool invalid_params = false;
	int idx;
	t_synctask_channels synctask;
	memset(&synctask,0,sizeof(t_synctask_channels));

	//convert params from structure
	if (input_tsc.has_adc_ch_cnt())	{
		synctask.adc_ch_cnt = input_tsc.adc_ch_cnt();
	} else invalid_params = true;

	if (input_tsc.has_sync_reg()) {
		synctask.sync_reg = input_tsc.sync_reg();
	} else invalid_params = true;

	if (input_tsc.adc_ch_idx_size() != synctask.adc_ch_cnt) invalid_params = true;
	else for (int i=0; i<input_tsc.adc_ch_idx_size(); i++)
				synctask.adc_ch_idx[i] = input_tsc.adc_ch_idx(i);

	if (invalid_params) {
		logMessage(LOG_LEVEL_CRITICAL,"Incoming SynctaskChannels isn't contain some of required fields\n");
		//logMessage(LOG_LEVEL_CRITICAL,"adc_ch_idx_size() = %d, adc_ch_cnt() = %d\n",input_tsc.adc_ch_idx_size(),input_tsc.adc_ch_cnt());
		return -EINVAL;
	}

	for (int i=0; i<synctask.adc_ch_cnt; i++) {
		idx = input_tsc.adc_ch_idx(i);
		//printf("i=%d, idx=%d\n",i,idx);
		if ((idx < 1) || (idx > p347_ADC_CHANNELS_CNT)) {
			logMessage(LOG_LEVEL_CRITICAL,"Invalid channel index (%d) in doStartSyncChannels\n",idx);
			return -EINVAL;
		}
		if (adcc[idx-1]->isRunning()) {
			logMessage(LOG_LEVEL_CRITICAL,"Channel %d is already running, cannot start doStartSyncChannels!\n",idx);
			return -EBUSY;
		}
		if (!adcc[idx-1]->isSetuped()) {
			logMessage(LOG_LEVEL_CRITICAL,"Channel %d is not setuped, cannot start!\n",idx);
			return -p347_ERROR_SYSTEM_CHANNEL_NOT_CONFIGURED;
		}
		adcc[input_tsc.adc_ch_idx(i) - 1]->prepareCopy();
		adcc[input_tsc.adc_ch_idx(i) - 1]->prepareRetranslate();
	}

	int ret = startSyncChannels(&synctask);

	return ret;
}

int ChannelManager::doStopSyncChannels(const channel_manager::SynctaskChannels & input_tsc, bool waitForWrite, bool waitForRetranslate) {
	bool invalid_params = false;
	t_synctask_channels synctask;
	memset(&synctask,0,sizeof(t_synctask_channels));

	//convert params from structure
	if (input_tsc.has_adc_ch_cnt())		synctask.adc_ch_cnt = input_tsc.adc_ch_cnt();
	if (input_tsc.has_sync_reg())		synctask.sync_reg = input_tsc.sync_reg();
	for (int i=0; i<input_tsc.adc_ch_idx_size(); i++)
		synctask.adc_ch_idx[i] = input_tsc.adc_ch_idx(i);

	if (invalid_params) {
		logMessage(LOG_LEVEL_CRITICAL,"Incoming SynctaskChannels isn't contains some of required fields\n");
		return -EINVAL;
	}

	int ret = stopSyncChannels(&synctask,waitForWrite,waitForRetranslate);

	return ret;
}
