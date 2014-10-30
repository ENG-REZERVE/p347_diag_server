/*
 * rcf_bind_channel_manager.hpp
 *
 *  Created on: Sep 21, 2014
 *      Author: ukicompile
 */

#ifndef RCF_BIND_CHANNEL_MANAGER_HPP_
#define RCF_BIND_CHANNEL_MANAGER_HPP_

#include <RCF/RCF.hpp>
//#include <RCFProto.hpp>
//#include "channel_manager.hpp"
#include "channel_manager.pb.h"
#include "task_manager.pb.h"

#define EMUL_TASK_TYPE_NONE				0
#define EMUL_TASK_TYPE_SPECTR			1
#define EMUL_TASK_TYPE_VHARMONIC		2
#define EMUL_TASK_TYPE_KURTOSIS			3
#define EMUL_TASK_TYPE_STATTIMESIG		4
#define EMUL_TASK_TYPE_PLAYER			5
#define EMUL_TASK_TYPE_VIBEG			6
#define EMUL_TASK_TYPE_QUALITY			7
#define EMUL_TASK_TYPE_SAVETIMESIG		8
#define EMUL_TASK_TYPE_VSENSORFROT		9
#define EMUL_TASK_TYPE_DECIM			10

RCF_BEGIN(CM_connection,"CM_connection")
	//---test
	RCF_METHOD_R1(int,remoteSleep, int)
	//------------------------------------------------------------------------------CHANNEL MANAGER
	RCF_METHOD_R2(int, createChannelManager, const channel_manager::ChannelManagerInitParams &, const task_manager::DSPEmulInitParams &)
	RCF_METHOD_R0(int, deleteChannelManager)

	RCF_METHOD_R0(channel_manager::ServerVersion,getServerVersion)
	RCF_METHOD_R0(channel_manager::AvailableChannels,getAvailableChannels)
	RCF_METHOD_R1(bool,isChannelAvailable,int)
	RCF_METHOD_R0(bool,isChannelsCreated)
	//---
	RCF_METHOD_R1(int,initMultiplexer, const channel_manager::MultiplexerInitParams &)
	RCF_METHOD_R0(int,deinitMultiplexer)
	//RCF_METHOD_R1(int,initChannelManager,const channel_manager::ChannelManagerInitParams &)
	//RCF_METHOD_R0(int,exitChannelManager)
	RCF_METHOD_R2(int,FPGADriverReload,const std::string &,const std::string &)
	RCF_METHOD_R0(int,checkFPGAStatus)
	//---Freq
	RCF_METHOD_R1(channel_manager::RotData,readRotData,unsigned char)
	RCF_METHOD_R1(int,doStartRotChannel,const channel_manager::RotChannelInitParams &)
	RCF_METHOD_R1(int,stopRotChannel,unsigned char)
	RCF_METHOD_R1(bool,isRotRunning,unsigned char)
	RCF_METHOD_R0(bool,isAnyRotChannelRunning)
	//---ADC
	RCF_METHOD_R1(bool,isADCRunning,unsigned char)
	RCF_METHOD_R0(bool,isAnyADCChannelRunning)
	RCF_METHOD_R1(int,doSetupAdcChannel,const channel_manager::ADCChannelInitParams &)
	RCF_METHOD_R1(int,startAdcChannel,unsigned char)
	RCF_METHOD_R3(int,stopAdcChannel,unsigned char, bool, bool)
	RCF_METHOD_R2(int,warmChannelStart,unsigned char, unsigned short)
	RCF_METHOD_R1(int,warmChannelEnd,unsigned char)
	RCF_METHOD_R1(int,doStartSyncChannels,const channel_manager::SynctaskChannels &)
	RCF_METHOD_R3(int,doStopSyncChannels,const channel_manager::SynctaskChannels &, bool, bool)
	RCF_METHOD_R0(channel_manager::ADCTimeOffsets,readADCTimeOffsets)
	//------------------------------------------------------------------------------MUX
	RCF_METHOD_R0(int,switchCommutatorOn)
	RCF_METHOD_R0(int,switchCommutatorOff)
	RCF_METHOD_R0(int,unmuxAll)
	RCF_METHOD_R2(int,doMuxChannel,int,int)
	//------------------------------------------------------------------------------DSPEMUL



	RCF_METHOD_R2(int,addTask,int,task_manager::AnyTaskParams &)
	RCF_METHOD_R2(int,delTask,int,int)
	RCF_METHOD_R4(int,clearTaskData,int,int,bool,bool)
	RCF_METHOD_R1(int,clearTaskList,int)
	RCF_METHOD_R3(int,clearTaskListData,int,bool,bool)
	RCF_METHOD_R1(int,clearData,int)
	RCF_METHOD_R1(int,clear,int)
	RCF_METHOD_R4(task_manager::AnyTaskResult,getTaskResult,int,int,int,int)
	RCF_METHOD_R2(int,getTaskStatus,int,int)
	RCF_METHOD_R2(int,getTaskProgress,int,int)
	RCF_METHOD_R2(int,getTaskNewResult,int,int)
	RCF_METHOD_R2(int,getTaskState,int,int)
	RCF_METHOD_R3(int,setTaskState,int,int,int)
	RCF_METHOD_R3(int,setTaskListState,int,const task_manager::IntArray &,int)
	RCF_METHOD_R2(int,getTotalTasksProgress,int,bool)
	RCF_METHOD_R1(bool,getAnyTasksStatusWaitingData,int)
	RCF_METHOD_R1(bool,getAllTasksStatusFinished,int)
	RCF_METHOD_R2(int,getTotalTasksLastErrorCode,int,bool)
	RCF_METHOD_R1(int,timesigOpen,int)
	RCF_METHOD_R1(int,timesigClose,int)
	RCF_METHOD_R1(bool,isTimesigOpened,int)
	RCF_METHOD_R1(int,getSigLength,int)
	RCF_METHOD_R1(int,getRotLength,int)
	RCF_METHOD_R1(double,getSensitivity,int)
	RCF_METHOD_R1(double,getGain,int)
	RCF_METHOD_R2(int,setSensitivity,int,double)
	RCF_METHOD_R2(int,setGain,int,double)
	//TODO: check, is it common for all channels or not
	RCF_METHOD_R2(int,setRotLabelsCount,int,int)
	RCF_METHOD_R1(int,getRotLabelsCount,int)
	//
	RCF_METHOD_R1(bool,getRotMetkasFromSig,int)
	RCF_METHOD_R2(int,setRotMetkasFromSig,int,bool)
	RCF_METHOD_R3(bool,startSaveTimeSig,int,const std::string &,int)
	RCF_METHOD_R1(int,stopSaveTimeSig,int)
	RCF_METHOD_R4(bool,startSaveTaskTimeSig,int,int,const std::string &,int)
	RCF_METHOD_R2(int,stopSaveTaskTimeSig,int,int)
	RCF_METHOD_R2(bool,saveTimeSig,int,const std::string &)

	RCF_METHOD_R1(long int,getBegSigOffSet,int)
	RCF_METHOD_R2(int,setBegSigOffSet,int,long int)
	RCF_METHOD_R1(long int,getBegRotOffSet,int)
	RCF_METHOD_R2(int,setBegRotOffSet,int,long int)

	RCF_METHOD_R1(int,getSrcSamplingFreq,int)
	RCF_METHOD_R2(int,setSrcSamplingFreq,int,int)
	RCF_METHOD_R1(int,getDecim,int)
	RCF_METHOD_R1(int,getInterp,int)

	//TODO: common?
	RCF_METHOD_R1(bool,getVibeg,int)
	RCF_METHOD_R2(int,setVibeg,int,bool);

	RCF_METHOD_R1(task_manager::AnyTaskParams,getVibegTaskParams,int)
	RCF_METHOD_R2(int,setVibegTaskParams,int,task_manager::VibegTaskParams &)

RCF_END(CM_connection)

RCF_BEGIN(PUB_connection,"PUB_connection")
	//---Publish
	RCF_METHOD_V1(void,alarmError,int)
	RCF_METHOD_V1(void,serverPing,int)
RCF_END(PUB_connection)

#endif /* RCF_BIND_CHANNEL_MANAGER_HPP_ */
