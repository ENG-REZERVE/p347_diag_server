/*
 * comm_center.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: ukicompile
 */

#include "comm_center.hpp"
#include "protobuf_convertors.h"

/*
#define CHECK_DSPEMUL_ENTITY if (dspemul == NULL) return -ENOENT; \
	TDSPEmul* d_link = dspemul->GetDSPEmul(emu_idx); \
	if (d_link == NULL) return -EINVAL
*/

#define CHECK_CHANNEL_INDEX(_input_idx_) if ((_input_idx_ < 1) || (_input_idx_ > p347_ADC_CHANNELS_CNT)) return -EINVAL; \
	if (channels_available[_input_idx_-1] == false) return -ENOENT

#define CHECK_DSPEMUL_ENTITY(_input_idx_) if ((_input_idx_ < 1) || (_input_idx_ > p347_ADC_CHANNELS_CNT)) return -EINVAL; \
	if (channels_available[_input_idx_-1] == false) return -ENOENT; \
	TDSPEmul* d_link = _getEmulByChannelIdx(_input_idx_); \
	if (d_link == NULL) return -EINVAL

#define CHECK_DSPEMUL_ENTITY_WITH_RET(_input_idx_) \
	TDSPEmul* d_link = NULL; \
	int ret = 0; \
	if ((emu_idx < 1) || (emu_idx > p347_ADC_CHANNELS_CNT)) ret = -EINVAL; \
	else { \
		if (channels_available[emu_idx-1] == false) { \
			ret = -ENOENT; \
		} else { \
			d_link = _getEmulByChannelIdx(emu_idx); \
			if (d_link == NULL) ret = -EINVAL; \
		} \
	}

CommCenter::CommCenter(const char* ip, int port, int* ch_avail, int* rot_avail) : UKI_LOG() {
	setPrefix("CommCenter");
	setConsoleLevel(LOG_LEVEL_FULL);
	int i;

	for (i=0; i<p347_ADC_CHANNELS_CNT; i++)
		channels_available[i] = true;
	for (i=0; i<p347_ROT_CHANNELS_CNT; i++)
			rot_available[i] = true;

	if (ch_avail != NULL) for (i=0; i<p347_ADC_CHANNELS_CNT; i++)
		if (ch_avail[i] <= 0) {
			channels_available[i] = false;
		}

	if (rot_avail != NULL) for (i=0; i<p347_ROT_CHANNELS_CNT; i++)
		if (rot_avail[i] <= 0) {
			rot_available[i] = false;
		}

	dspemul = NULL;
	cman = NULL;
	test_counter = 0;

	if (RCF::getInitRefCount() <= 0)
		RCF::init();

	server = new RCF::RcfServer(RCF::TcpEndpoint(ip,port));
	assert(server != NULL);

	server->bind<CM_connection>(*this);

	server->start();

	pubPtr = server->createPublisher<PUB_connection>();

	logMessage(LOG_LEVEL_FULL,"Comm center start complete!\n");
}

CommCenter::~CommCenter() {
	logMessage(LOG_LEVEL_FULL,"deleting...\n");
	pubPtr->close();
	server->stop();
	deleteChannelManager();

	delete server;
	logMessage(LOG_LEVEL_FULL,"destructor complete!\n");
}

//------------------------------------------------------------------------
int CommCenter::remoteSleep(int input_parameter) {
	int ret = test_counter;
	test_counter++;
	logMessage(LOG_LEVEL_FULL,"remoteSleep(%d) cnt=%d\n",input_parameter,ret);
	sleep(input_parameter);
	return ret;
}

//------------------------------------------------------------------------

int CommCenter::createChannelManager(const channel_manager::ChannelManagerInitParams par, const task_manager::DSPEmulInitParams dspip) {
	if (cman != NULL) {
		logMessage(LOG_LEVEL_CRITICAL,"Looks like Channel Manager is already created!\n");
		return -EBUSY;
	}
	if (dspemul != NULL) {
		logMessage(LOG_LEVEL_CRITICAL,"Looks like DSPEmul is already created!\n");
		return -EBUSY;
	}
	if (_getAvailableChannelsNumber() <= 0) {
		return -ENOENT;
	}

	//-----------------------------------------------------------------------------------CM
	cman = new ChannelManager();
	if (cman == NULL) {
		logMessage(LOG_LEVEL_CRITICAL,"Failed to create Channel Manager\n");
		return -ENOMEM;
	}
	logMessage(LOG_LEVEL_FULL,"Channel Manager created\n");

	int ret = cman->initManager(par);
	if (ret != 0) {
		delete cman;
		cman = NULL;
		return ret;
	}
	//-----------------------------------------------------------------------------------DE
	ret = _createDSPEmul(dspip);
	if (ret != 0) {
		delete cman;
		cman = NULL;
		return ret;
	}
	//-----------------------------------------------------------------------------------BIND
	int emu_idx = 0;
	for (int i=0; i<p347_ADC_CHANNELS_CNT; i++) if (channels_available[i] == true) {
		ret = _bindChannelToEmul(i+1,emu_idx);
		if (ret != 0) {
			delete cman;
			cman = NULL;
			delete dspemul;
			dspemul = NULL;

			return ret;
		}
		emu_idx++;
	}

	connection_alarm = cman->sigAlarm.connect(boost::bind(&CommCenter::onSigAlarm,this,_1));

	return 0;
}

int CommCenter::deleteChannelManager() {
	_deleteDSPEmul();

	if (cman == NULL) return -ENOENT;

	connection_alarm.disconnect();

	delete cman;
	cman = NULL;
	logMessage(LOG_LEVEL_FULL,"Channel Manager deleted\n");
	return 0;
}

bool CommCenter::isChannelAvailable(int ch_idx) {
	if ((ch_idx < 1) || (ch_idx > p347_ADC_CHANNELS_CNT))
		return false;

	return channels_available[ch_idx-1];
}

bool CommCenter::isChannelsCreated() {
	if (cman == NULL) return false;
	if (dspemul == NULL) return false;
	return true;
}

channel_manager::AvailableChannels CommCenter::getAvailableChannels(void) {
	channel_manager::AvailableChannels ac;

	ac.set_error_code(0);

	for (int i=0; i<p347_ADC_CHANNELS_CNT; i++) {
		if (channels_available[i]) {
			ac.add_channel_idx(i+1);
			printf("add available adc index %d");
		}
	}

	for (int i=0; i<p347_ROT_CHANNELS_CNT; i++) {
		if (rot_available[i]) {
			ac.add_rot_idx(i+1);
			printf("add available rot index %d");
		}
	}

	return ac;
}

channel_manager::ServerVersion CommCenter::getServerVersion(void) {
	channel_manager::ServerVersion sv_ret;

	sv_ret.CopyFrom(sv);
	printf("getServerVersion: %d.%d b%s",sv_ret.major(),sv_ret.minor(),sv_ret.build().c_str());

	return sv_ret;
}

void CommCenter::setServerVersion(int major, int minor, std::string build) {
	sv.set_error_code(0);
	sv.set_major(major);
	sv.set_minor(minor);
	sv.set_build(build);
}

//------------------------------------------------------------------------

int CommCenter::initMultiplexer(const channel_manager::MultiplexerInitParams & mip) {
	//TODO
	logMessage(LOG_LEVEL_CRITICAL,"initMultiplexer called, not implemented\n");
	return 0;
}

int CommCenter::deinitMultiplexer() {
	//TODO
	logMessage(LOG_LEVEL_CRITICAL,"deinitMultiplexer called, not implemented\n");
	return 0;
}

//------------------------------------------------------------------------
/*
int CommCenter::initChannelManager(const channel_manager::ChannelManagerInitParams par) {
	if (cman == NULL) return -ENOENT;
	return cman->initManager(par);
}

int CommCenter::exitChannelManager() {
	if (cman == NULL) return -ENOENT;

	cman->exitManager();
	return 0;
}
*/
int CommCenter::FPGADriverReload(const std::string old_name, const std::string new_path) {
	if (cman == NULL) return -ENOENT;
	return cman->FPGADriverReload(old_name,new_path);
}

int CommCenter::checkFPGAStatus() {
	if (cman == NULL) return -ENOENT;
		return cman->checkFPGAStatus();
}

//-----------------------------------------------------------------------------Freq
channel_manager::RotData CommCenter::readRotData(unsigned char ch_idx) {
	if (cman == NULL) {
		channel_manager::RotData send_rot;
		send_rot.set_error_code(-ENOENT);
		return send_rot;
	}

	return cman->readRotData(ch_idx);
};

int CommCenter::doStartRotChannel(channel_manager::RotChannelInitParams & rcip) {
	if (cman == NULL) return -ENOENT;
	return cman->doStartRotChannel(rcip);
}

int CommCenter::stopRotChannel(unsigned char ch_idx) {
	if (cman == NULL) return -ENOENT;
	return cman->stopRotChannel(ch_idx);
}

bool CommCenter::isRotRunning(unsigned char ch_idx) {
	if (cman == NULL) return false;
	return cman->isRotRunning(ch_idx);
}

bool CommCenter::isAnyRotChannelRunning() {
	if (cman == NULL) return false;
	return cman->isAnyRotChannelRunning();
}

//-----------------------------------------------------------------------------ADC

bool CommCenter::isADCRunning(unsigned char ch_idx) {
	if (cman == NULL) return false;
	if ((ch_idx < 1) || (ch_idx > p347_ADC_CHANNELS_CNT)) return false;

	return cman->adcc[ch_idx-1]->isRunning();
}

bool CommCenter::isAnyADCChannelRunning() {
	if (cman == NULL) return false;
	return cman->isAnyADCChannelRunning();
}

int CommCenter::doSetupAdcChannel(channel_manager::ADCChannelInitParams & adccip) {
	if (cman == NULL) return -ENOENT;
	return cman->doSetupAdcChannel(adccip);
}

int CommCenter::startAdcChannel(unsigned char ch_idx) {
	if (cman == NULL) return -ENOENT;
	return cman->doStartAdcChannel(ch_idx);
}

int CommCenter::stopAdcChannel(unsigned char ch_idx, bool wfw, bool wfr) {
	if (cman == NULL) return -ENOENT;
	return cman->stopAdcChannel(ch_idx, wfw, wfr);
}

int CommCenter::warmChannelStart(unsigned char ch_idx, unsigned short reg_settings) {
	if (cman == NULL) return -ENOENT;
	return cman->warmChannelStart(ch_idx,reg_settings);
}

int CommCenter::warmChannelEnd(unsigned char ch_idx) {
	if (cman == NULL) return -ENOENT;
	return cman->warmChannelEnd(ch_idx);
}

int CommCenter::doStartSyncChannels(const channel_manager::SynctaskChannels & tsc) {
	if (cman == NULL) return -ENOENT;
	return cman->doStartSyncChannels(tsc);
}

int CommCenter::doStopSyncChannels(const channel_manager::SynctaskChannels & tsc, bool wfw, bool wfr) {
	if (cman == NULL) return -ENOENT;
	return cman->doStopSyncChannels(tsc,wfw,wfr);
}

channel_manager::ADCTimeOffsets CommCenter::readADCTimeOffsets() {
	if (cman == NULL) {
		channel_manager::ADCTimeOffsets adcto;
		adcto.set_error_code(-ENOENT);
		return adcto;
	} else
		return cman->readADCTimeOffsets();
}

//-----------------------------------------------------------------------------MUX

int CommCenter::switchCommutatorOn() {
	if (cman == NULL) return -ENOENT;
	return cman->switchCommutatorOn();
}

int CommCenter::switchCommutatorOff() {
	if (cman == NULL) return -ENOENT;
	return cman->switchCommutatorOff();
}

int CommCenter::unmuxAll() {
	if (cman == NULL) return -ENOENT;
	return cman->unmuxAll();
}

int CommCenter::doMuxChannel(int phys_ch,int mux_ch) {
	if (cman == NULL) return -ENOENT;
	return cman->doMuxChannel(phys_ch,mux_ch);
}

//-----------------------------------------------------------------------------DSPEmul

int CommCenter::addTask(int emu_idx,task_manager::AnyTaskParams & task_params) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	//iterate over task types
	if (task_params.has_spect_par()) {
		task_manager::SpectrTaskParams* inparam = task_params.mutable_spect_par();
		spectrtaskparams_t outparam;

		int ret = extractSpectrTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractSpectrTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
	}

	if (task_params.has_harmonic_par()) {
		task_manager::VharmonicTaskParams* inparam = task_params.mutable_harmonic_par();
		vharmonicstaskparams_t outparam;

		int ret = extractVharmonicTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractVharmonicTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
	}

	if (task_params.has_kurtosis_par()) {
		task_manager::StatKurtosisTaskParams* inparam = task_params.mutable_kurtosis_par();
		statkurtosistaskparams_t outparam;

		int ret = extractStatKurtosisTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractStatKurtosisTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
	}

	if (task_params.has_stattimesig_par()) {
		task_manager::StatTimeSigTaskParams* inparam = task_params.mutable_stattimesig_par();
		stattimesigtaskparams_t outparam;

		int ret = extractStatTimesigTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractStatTimesigTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
	}

	if (task_params.has_player_par()) {
		task_manager::PlayerTimeSigTaskParams* inparam = task_params.mutable_player_par();
		playertimesigtaskparams_t outparam;

		int ret = extractPlayerTimesigTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractPlayerTimesigTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
	}

	if (task_params.has_vibeg_par()) {
		logMessage(LOG_LEVEL_CRITICAL,"No AddTask possible for Vibeg, use setVibegTaskParams instead!\n");
		return -EINVAL;
		/*
		task_manager::VibegTaskParams* inparam = task_params.mutable_vibeg_par();
		vibegtaskparams_t outparam;

		int ret = extractVibegTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractVibegTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
		*/
	}

	if (task_params.has_quality_par()) {
		task_manager::QualityTimeSigTaskParams* inparam = task_params.mutable_quality_par();
		qualitytimesigtaskparams_t outparam;

		int ret = extractQualityTimesigTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractQualityTimesigTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
	}

	if (task_params.has_savetimesig_par()) {
		logMessage(LOG_LEVEL_CRITICAL,"No AddTask possible for SaveTimeSig task!\n");
		return -EINVAL;
		/*
		task_manager::SaveTimeSigTaskParams* inparam = task_params.mutable_savetimesig_par();
		savetimesigtaskparams_t outparam;

		int ret = extractSaveTimesigTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractSaveTimesigTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
		*/
	}

	if (task_params.has_vsensorfrot_par()) {
		task_manager::VsensorFrotTaskParams* inparam = task_params.mutable_vsensorfrot_par();
		vsensorfrottaskparams_t outparam;

		int ret = extractVsensorFrotTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractVsensorFrotTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
	}

	if (task_params.has_decim_par()) {
		logMessage(LOG_LEVEL_CRITICAL,"No AddTask possible for Decim task!\n");
		return -EINVAL;
		/*
		task_manager::DecimTimeSigTaskParams* inparam = task_params.mutable_decim_par();
		decimtimesigtaskparams_t outparam;

		int ret = extractDecimTimesigTaskParams(inparam,&outparam);
		if (ret < 0) {
			logMessage(LOG_LEVEL_CRITICAL,"extractDecimTimesigTaskParams failed with %d\n",ret);
			return ret;
		}

		return d_link->AddTask(outparam);
		*/
	}

	logMessage(LOG_LEVEL_CRITICAL,"Cannot found any task params in incoming message!\n");
	return -ERANGE; //TODO: maybe another code?
}

int CommCenter::delTask(int emu_idx,int task_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->DelTask(task_idx); //TODO: how to check task existence?

	return 0;
}

int CommCenter::clearTaskData(int emu_idx,int task_idx,bool AClearOffSet,bool AClearResults) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->ClearTaskData(task_idx,AClearOffSet,AClearResults);

	return 0;
}

int CommCenter::clearTaskList(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->ClearTaskList();

	return 0;
}

int CommCenter::clearTaskListData(int emu_idx,bool AClearOffSet,bool AClearResults) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->ClearTaskListData(AClearOffSet,AClearResults);

	return 0;
}

int CommCenter::clearData(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->ClearData();

	return 0;
}

int CommCenter::clear(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->Clear();

	return 0;
}

task_manager::AnyTaskResult CommCenter::getTaskResult(int emu_idx,int task_idx,int supposed_type,int res_code) {
	task_manager::AnyTaskResult		atr;

	//printf("getTaskResult(emu_idx=%d, task_idx=%d, supposed_type=%d, res_code= %d)\n",emu_idx,task_idx,supposed_type,res_code);

	if ((supposed_type <= EMUL_TASK_TYPE_NONE) || (supposed_type > EMUL_TASK_TYPE_DECIM)
			|| (supposed_type == EMUL_TASK_TYPE_SPECTR)
			|| (supposed_type == EMUL_TASK_TYPE_PLAYER)
			|| (supposed_type == EMUL_TASK_TYPE_VIBEG)) {
		logMessage(LOG_LEVEL_CRITICAL,"No data result possible for emul task type %d\n",supposed_type);
		atr.set_error_code(-EINVAL);
		return atr;
	}

	TDSPEmul* d_link = NULL;

	if ((dspemul == NULL) || (cman == NULL)) {
		atr.set_error_code(-ENOENT);
	} else {
		CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);

		if (ret != 0) atr.set_error_code(ret);
		else {
			void* some_result = d_link->GetTaskResult(task_idx,res_code);
			if (some_result == NULL) {
				atr.set_error_code(-ENODATA);
			} else {
				switch (supposed_type) {
					case EMUL_TASK_TYPE_SPECTR: {
						try {
							TSimpleSpectr* sp = dynamic_cast<TSimpleSpectr*>((TCustomSpectr*)some_result);

							if (sp == NULL) {
								logMessage(LOG_LEVEL_CRITICAL,"NULL spectr result\n");
								atr.set_error_code(-ENODATA); //TODO: apply another error code
							} else {
								task_manager::Spectr_data* sd = atr.mutable_spectr_res();
								int ret = packSpectrResult(sd,sp);
								atr.set_error_code(ret);
								delete sp;
							}
						}
						catch (const std::bad_cast& e) {
							logMessage(LOG_LEVEL_CRITICAL,"dynamic cast failed for spectr result: %s\n",e.what());
							logMessage(LOG_LEVEL_CRITICAL,"Cannot delete result, so memory leak supposed\n");
							atr.set_error_code(-EBADMSG); //TODO: apply another error code
						}
					break; }
					case EMUL_TASK_TYPE_VHARMONIC: {
						vharmonic_t* vharmonic_result = (vharmonic_t*)some_result;
						task_manager::Vharmonic_data* vhd = atr.mutable_harmonic_res();
						int ret = packVharmonicResult(vhd,vharmonic_result);
						atr.set_error_code(ret);

						delete vharmonic_result;
					break; }
					case EMUL_TASK_TYPE_KURTOSIS: {
						statkurtosis_t* kurtosis_result = (statkurtosis_t*)some_result;
						task_manager::StatKurtosis_data* vhd = atr.mutable_kurtosis_res();
						int ret = packKurtosisResult(vhd,kurtosis_result);
						atr.set_error_code(ret);

						delete kurtosis_result;
					break; }
					case EMUL_TASK_TYPE_STATTIMESIG: {
						stattimesig_t* stattimesig_result = (stattimesig_t*)some_result;
						task_manager::StatTimeSig_data* vhd = atr.mutable_stattimesig_res();
						int ret = packStattimesigResult(vhd,stattimesig_result);
						atr.set_error_code(ret);

						delete stattimesig_result;
					break; }
					case EMUL_TASK_TYPE_QUALITY: {
						qualitytimesig_t* quality_result = (qualitytimesig_t*)some_result;
						task_manager::QualityTimeSig_data* vhd = atr.mutable_quality_res();
						int ret = packQualitytimesigResult(vhd,quality_result);
						atr.set_error_code(ret);

						delete quality_result;
					break; }
					case EMUL_TASK_TYPE_VSENSORFROT: {
						vsensorfrot_t* vsensorfrot_result = (vsensorfrot_t*)some_result;
						task_manager::VsensorFrot_data* vhd = atr.mutable_vsensorfrot_res();
						int ret = packVsensorfrotResult(vhd,vsensorfrot_result);
						atr.set_error_code(ret);

						delete vsensorfrot_result;
					break; }
					case EMUL_TASK_TYPE_DECIM: {
						decimtimesig_t* decim_result = (decimtimesig_t*)some_result;
						task_manager::DecimTimeSig_data* vhd = atr.mutable_decim_res();
						int ret = packDecimtimesigResult(vhd,decim_result);
						atr.set_error_code(ret);

						delete decim_result;
					break; }
				}
			}
		}
	}

	return atr;
}

int CommCenter::getTaskStatus(int emu_idx,int task_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetTaskStatus(task_idx);
}

int CommCenter::getTaskProgress(int emu_idx,int task_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetTaskProgress(task_idx);
}

int CommCenter::getTaskNewResult(int emu_idx,int task_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetTaskNewResult(task_idx);
}

int CommCenter::getTaskState(int emu_idx,int task_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetTaskState(task_idx);
}

int CommCenter::setTaskState(int emu_idx,int task_idx,int new_state) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetTaskState(task_idx,new_state);

	return 0;
}

int CommCenter::setTaskListState(int emu_idx,const task_manager::IntArray & task_idx_list, int new_state) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	int sz = task_idx_list.values_size();
	if (sz <= 0) return -EINVAL;

	int* idlist = new int[sz];
	for (int i=0; i<sz; i++)
		idlist[i] = task_idx_list.values(i);

	d_link->SetTaskListState(idlist,sz,new_state);

	delete[] idlist;

	return 0;
}

int CommCenter::getTotalTasksProgress(int emu_idx,bool AExceptMonitoring) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetTotalTasksProgress(AExceptMonitoring);
}

bool CommCenter::getAnyTasksStatusWaitingData(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetAnyTasksStatusWaitingData();
}

bool CommCenter::getAllTasksStatusFinished(int emu_idx) {
	//CHECK_DSPEMUL_ENTITY(emu_idx);
	CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
	if (ret != 0) return false;

	return d_link->GetAllTasksStatusFinished();
}

int CommCenter::getTotalTasksLastErrorCode(int emu_idx,bool AExceptMonitoring) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetTotalTasksLastErrorCode(AExceptMonitoring);
}

int CommCenter::timesigOpen(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->Open();
}

int CommCenter::timesigClose(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->Close();

	return 0;
}

bool CommCenter::isTimesigOpened(int emu_idx) {
	CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
	if (ret != 0) return false;

	return d_link->GetOpened();
}

int CommCenter::getSigLength(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetSigLength();
}

int CommCenter::getRotLength(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetRotLength();
}

double CommCenter::getSensitivity(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetSensitivity();
}

double CommCenter::getGain(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetGain();
}

int CommCenter::setSensitivity(int emu_idx,double ASensitivity) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetSensitivity(ASensitivity);

	return 0;
}

int CommCenter::setGain(int emu_idx,double AGain) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetGain(AGain);

	return 0;
}

int CommCenter::setRotLabelsCount(int emu_idx,int ARotLabelsCount) {
	CHECK_DSPEMUL_ENTITY(emu_idx);
	if (ARotLabelsCount <= 0) return -EINVAL;

	d_link->SetRotLabelsCount(ARotLabelsCount);

	return 0;
}

int CommCenter::getRotLabelsCount(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetRotLabelsCount();
}

bool CommCenter::getRotMetkasFromSig(int emu_idx) {
	CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
	if (ret != 0) return false;

	return d_link->GetRotMetkasFromSig();
}

int CommCenter::setRotMetkasFromSig(int emu_idx, bool ARotMetkasFromSig) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetRotMetkasFromSig(ARotMetkasFromSig);

	return 0;
}

bool CommCenter::startSaveTimeSig(int emu_idx,const std::string AFileNameForSave,int ATSDTCForSave) {
	CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
	if (ret != 0) return false;

	return d_link->StartSaveTimeSig((char *)AFileNameForSave.c_str(),ATSDTCForSave);
}

int CommCenter::stopSaveTimeSig(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->StopSaveTimeSig();

	return 0;
}

bool CommCenter::startSaveTaskTimeSig(int emu_idx,int task_idx,const std::string AFileNameForSave,int ATSDTCForSave) {
	CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
	if (ret != 0) return false;

	return d_link->StartSaveTimeSig(task_idx,(char *)AFileNameForSave.c_str(),ATSDTCForSave);
}

int CommCenter::stopSaveTaskTimeSig(int emu_idx,int task_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->StopSaveTimeSig(task_idx);

	return 0;
}

bool CommCenter::saveTimeSig(int emu_idx,const std::string AFileName) {
	CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
	if (ret != 0) return false;

	return d_link->SaveTimeSig((char *)AFileName.c_str());
}

long int CommCenter::getBegSigOffSet(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetBegSigOffSet();
}

int CommCenter::setBegSigOffSet(int emu_idx,long int ABegSigOffSet) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetBegSigOffSet(ABegSigOffSet);

	return 0;
}

long int CommCenter::getBegRotOffSet(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetBegRotOffSet();
}

int CommCenter::setBegRotOffSet(int emu_idx,long int ABegRotOffSet) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetBegRotOffSet(ABegRotOffSet);

	return 0;
}

int CommCenter::getSrcSamplingFreq(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetSrcSamplingFreq();
}

int CommCenter::setSrcSamplingFreq(int emu_idx,int ASrcSamplingFreq) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetSrcSamplingFreq(ASrcSamplingFreq);

	return 0;
}

int CommCenter::getDecim(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetDecim();
}

int CommCenter::getInterp(int emu_idx) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	return d_link->GetInterp();
}

bool CommCenter::getVibeg(int emu_idx) {
	CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
	if (ret != 0) return false;

	return d_link->GetVibeg();
}

int CommCenter::setVibeg(int emu_idx,bool AVibeg) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	d_link->SetVibeg(AVibeg);

	return 0;
}


task_manager::AnyTaskParams CommCenter::getVibegTaskParams(int emu_idx) {
	task_manager::AnyTaskParams		atp;

	if (dspemul == NULL) {
		atp.set_error_code(-ENOENT);
	} else {
		//TDSPEmul* d_link = dspemul->GetDSPEmul(emu_idx);
		//if (d_link == NULL) atp.set_error_code(-EINVAL);
		CHECK_DSPEMUL_ENTITY_WITH_RET(emu_idx);
		if (ret != 0) {
			atp.set_error_code(-EINVAL);
		} else {
			task_manager::VibegTaskParams* vp = atp.mutable_vibeg_par();
			vibegtaskparams_t rd = d_link->GetVibegTaskParams();

			int ret = packVibegTaskParams(vp,&rd);
			atp.set_error_code(ret);
		}
	}

	return atp;
}

int CommCenter::setVibegTaskParams(int emu_idx,task_manager::VibegTaskParams & AVTP) {
	CHECK_DSPEMUL_ENTITY(emu_idx);

	vibegtaskparams_t vtp;
	int ret = extractVibegTaskParams(&AVTP,&vtp);
	if (ret < 0) return ret;

	d_link->SetVibegTaskParams(vtp);

	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------Publish

void CommCenter::publishCast(int param) {
	pubPtr->publish().serverPing(param);
}

void CommCenter::onSigAlarm(int error_code) {
	pubPtr->publish().alarmError(error_code);
}

//------------------------------------------------------------------------------Internal

int CommCenter::_bindChannelToEmul(int ch_idx, int emu_idx) {
	if (cman == NULL) return -ENOENT;
	if (dspemul == NULL) return -ENOENT;
	CHECK_CHANNEL_INDEX(ch_idx);
	TDSPEmul* d_link = dspemul->GetDSPEmul(emu_idx);
	if (d_link == NULL) return -EINVAL;

	return cman->adcc[ch_idx-1]->assignDSPEmul(d_link);
}

TDSPEmul* CommCenter::_getEmulByChannelIdx(int ch_idx) {
	//printf("_getEmulByChannelIdx %d avail = %d, cman=%p\n",ch_idx, channels_available[ch_idx-1],cman);
	if (cman == NULL) return NULL;
	if ((ch_idx < 1) || (ch_idx > p347_ADC_CHANNELS_CNT)) return NULL;
	if (channels_available[ch_idx-1] == false) return NULL;

	//printf("_getEmulByChannelIdx found %p\n",cman->adcc[ch_idx-1]->getEmulLink());

	return cman->adcc[ch_idx-1]->getEmulLink();
}

int CommCenter::_createDSPEmul(const task_manager::DSPEmulInitParams & dspip) {
	dspemul = new TDSPEmulMCh<TDSPEmul>(dspip.atsdtc(),dspip.ainitsigbufferlength(),dspip.asigbufferincrement(),
										dspip.ainitrotbufferlength(),dspip.arotbufferincrement(),_getAvailableChannelsNumber());

	if (dspemul == NULL) {
		logMessage(LOG_LEVEL_CRITICAL,"Failed to create DSP Emul\n");
		return -ENOMEM;
	}

	logMessage(LOG_LEVEL_FULL,"DSPEmul created\n");
	return 0;
}

int CommCenter::_deleteDSPEmul() {
	if (dspemul != NULL) {
		delete dspemul;
		dspemul = NULL;
	} else {
		return -ENOENT;
	}
	logMessage(LOG_LEVEL_MAIN,"deleteDSPEmul complete\n");
	return 0;
}

int CommCenter::_getAvailableChannelsNumber() {
	int ret = 0;

	for (int i=0; i<p347_ADC_CHANNELS_CNT; i++) {
		if (channels_available[i]) {
			ret++;
		}
	}

	return ret;
}


