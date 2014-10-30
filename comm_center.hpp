/*
 * comm_center.hpp
 *
 *  Created on: Sep 29, 2014
 *      Author: ukicompile
 */

#ifndef COMM_CENTER_HPP_
#define COMM_CENTER_HPP_

#include "rcf_bind_channel_manager.hpp"
#include "channel_manager.hpp"
#include "DSPEmulMCh.h"

typedef RCF::Publisher<PUB_connection> PUB_connection_publisher;
typedef boost::shared_ptr<PUB_connection_publisher> t_shared_publisher_pointer;

//CommCenter manages server's side of connection with interface
//and ties all of remote call into one entity (CM_connection)
class CommCenter : public UKI_LOG {
public:
	CommCenter(const char* ip, int port, int* ch_avail, int* rot_avail);
	~CommCenter();
	//test
	int remoteSleep(int input_parameter);
	//---------------------------------------------------------------------wrapping
	//-----------------------------------------------------------------------------
	int createChannelManager(const channel_manager::ChannelManagerInitParams par, const task_manager::DSPEmulInitParams dspip);
	int deleteChannelManager();

	channel_manager::AvailableChannels getAvailableChannels(void);
	channel_manager::ServerVersion getServerVersion(void);

	bool isChannelAvailable(int ch_idx);
	bool isChannelsCreated();
	//---------------------------------------------------
	int initMultiplexer(const channel_manager::MultiplexerInitParams & mip);
	int deinitMultiplexer();
	//---------------------------------------------------
	//int initChannelManager(const channel_manager::ChannelManagerInitParams par);
	//int exitChannelManager();
	int FPGADriverReload(const std::string old_name, const std::string new_path);
	int checkFPGAStatus(void);
	//---------------------------------------------------
	channel_manager::RotData readRotData(unsigned char ch_idx);
	int doStartRotChannel(channel_manager::RotChannelInitParams & rcip);
	int stopRotChannel(unsigned char ch_idx);
	bool isRotRunning(unsigned char ch_idx);
	bool isAnyRotChannelRunning();
	//---------------------------------------------------
	bool isADCRunning(unsigned char ch_idx);
	bool isAnyADCChannelRunning();
	int doSetupAdcChannel(channel_manager::ADCChannelInitParams & adccip);
	int startAdcChannel(unsigned char ch_idx);
	int stopAdcChannel(unsigned char ch_idx, bool wfw, bool wfr);
	int warmChannelStart(unsigned char ch_idx, unsigned short reg_settings);
	int warmChannelEnd(unsigned char ch_idx);
	int doStartSyncChannels(const channel_manager::SynctaskChannels & tsc);
	int doStopSyncChannels(const channel_manager::SynctaskChannels & tsc, bool wfw, bool wfr);
	channel_manager::ADCTimeOffsets readADCTimeOffsets();
	//----------------------------------------------------
	int switchCommutatorOn();
	int switchCommutatorOff();
	int unmuxAll();
	int doMuxChannel(int phys_ch,int mux_ch);
	//----------------------------------------------------

	//-----------------------------------------------------------------------------
	int addTask(int emu_idx,task_manager::AnyTaskParams & task_params);
	int delTask(int emu_idx,int task_idx);
	int clearTaskData(int emu_idx,int task_idx,bool AClearOffSet,bool AClearResults);
	int clearTaskList(int emu_idx);
	int clearTaskListData(int emu_idx,bool AClearOffSet,bool AClearResults);
	int clearData(int emu_idx);
	int clear(int emu_idx);
	task_manager::AnyTaskResult getTaskResult(int emu_idx,int task_idx,int supposed_type,int res_code);
	int getTaskStatus(int emu_idx,int task_idx);
	int getTaskProgress(int emu_idx,int task_idx);
	int getTaskNewResult(int emu_idx,int task_idx);
	int getTaskState(int emu_idx,int task_idx);
	int setTaskState(int emu_idx,int task_idx,int new_state);
	int setTaskListState(int emu_idx,const task_manager::IntArray & task_idx_list, int new_state);
	int getTotalTasksProgress(int emu_idx,bool AExceptMonitoring);
	bool getAnyTasksStatusWaitingData(int emu_idx);
	bool getAllTasksStatusFinished(int emu_idx);
	int getTotalTasksLastErrorCode(int emu_idx,bool AExceptMonitoring);
	int timesigOpen(int emu_idx);
	int timesigClose(int emu_idx);
	bool isTimesigOpened(int emu_idx);
	int getSigLength(int emu_idx);
	int getRotLength(int emu_idx);
	double getSensitivity(int emu_idx);
	double getGain(int emu_idx);
	int setSensitivity(int emu_idx,double ASensitivity);
	int setGain(int emu_idx,double AGain);
	int setRotLabelsCount(int emu_idx,int ARotLabelsCount);
	int getRotLabelsCount(int emu_idx);
	bool getRotMetkasFromSig(int emu_idx);
	int setRotMetkasFromSig(int emu_idx, bool ARotMetkasFromSig);
	bool startSaveTimeSig(int emu_idx,const std::string AFileNameForSave,int ATSDTCForSave);
	int stopSaveTimeSig(int emu_idx);
	bool startSaveTaskTimeSig(int emu_idx,int task_idx,const std::string AFileNameForSave,int ATSDTCForSave);
	int stopSaveTaskTimeSig(int emu_idx,int task_idx);
	bool saveTimeSig(int emu_idx,const std::string AFileName);

	long int getBegSigOffSet(int emu_idx);
	int setBegSigOffSet(int emu_idx,long int ABegSigOffSet);
	long int getBegRotOffSet(int emu_idx);
	int setBegRotOffSet(int emu_idx,long int ABegRotOffSet);

	int getSrcSamplingFreq(int emu_idx);
	int setSrcSamplingFreq(int emu_idx,int ASrcSamplingFreq);
	int getDecim(int emu_idx);
	int getInterp(int emu_idx);

		//TODO: common?
	bool getVibeg(int emu_idx);
	int setVibeg(int emu_idx,bool AVibeg);

	task_manager::AnyTaskParams getVibegTaskParams(int emu_idx);
	int setVibegTaskParams(int emu_idx,task_manager::VibegTaskParams & AVTP);

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void setServerVersion(int major, int minor, std::string build);
	void publishCast(int param);

	void onSigAlarm(int alarm_code);
	//-----------------------------------------------------------------------------
private:
	int _bindChannelToEmul(int ch_idx, int emu_idx);
	TDSPEmul* _getEmulByChannelIdx(int ch_idx);
	int _createDSPEmul(const task_manager::DSPEmulInitParams & dspip);
	int _deleteDSPEmul();
	int _getAvailableChannelsNumber();
	//int _getEmulIdxByChannelIdx(int ch_idx);

	RCF::RcfServer*				server;
	ChannelManager* 			cman;
	t_shared_publisher_pointer 	pubPtr;
	int							test_counter;
	bool 						channels_available[p347_ADC_CHANNELS_CNT];
	bool 						rot_available[p347_ROT_CHANNELS_CNT];
	TDSPEmulMCh<TDSPEmul>*		dspemul;

	channel_manager::ServerVersion sv;

	boost::signals2::connection			connection_alarm;
};



#endif /* COMM_CENTER_HPP_ */
