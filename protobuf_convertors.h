/*
 * protobuf_convertors.hpp
 *
 *  Created on: Oct 9, 2014
 *      Author: ukicompile
 */

#ifndef PROTOBUF_CONVERTORS_HPP_
#define PROTOBUF_CONVERTORS_HPP_

// "extract" = convert from protobuf to original state
// "pack" = convert from original state to protobuf


#include "task_manager.pb.h"
#include "channel_manager.pb.h"
#include "hardware_monitor.pb.h"
#include "DSPEmulDefines.h"
#include "p347_pwr_user.h"
#include "TSpectr.h"

extern int extractLPFilter(task_manager::LPFilter* proto_src, TLPFilter* data_dst);
extern int extractBPFilter(task_manager::BPFilter* proto_src, TBPFilter* data_dst);

extern int packLPFilter(task_manager::LPFilter* proto_dst, TLPFilter* data_src);
extern int packBPFilter(task_manager::BPFilter* proto_dst, TBPFilter* data_src);

//--------------------------------------------------------------------------------------------------------------------------------

extern int extractProtoCMN(task_manager::CommonTaskParams* proto_src, commontaskparams_t* data_dst);
extern int extractLPFilterParams(task_manager::LPFilter_params* proto_src, LPFParams_t* data_dst);
extern int extractBPFilterParams(task_manager::BPFilter_params* proto_src, BPFParams_t* data_dst);
extern int extractRSMPFilterParams(task_manager::RSMPFilter_params* proto_src, RSMPFParams_t* data_dst);
extern int extractHPFilterParams(task_manager::HPFilter_params* proto_src, HPFParams_t* data_dst);
extern int extractSPFilterParams(task_manager::SPFilter_params* proto_src, SPFParams_t* data_dst);
extern int extractINTGFilterParams(task_manager::INTGFilter_params* proto_src, INTGFParams_t* data_dst);

extern int extractSpectrTaskParams(task_manager::SpectrTaskParams* proto_src, spectrtaskparams_t* data_dst);
extern int extractVharmonicTaskParams(task_manager::VharmonicTaskParams* proto_src, vharmonicstaskparams_t* data_dst);
extern int extractStatKurtosisTaskParams(task_manager::StatKurtosisTaskParams* proto_src, statkurtosistaskparams_t* data_dst);
extern int extractStatTimesigTaskParams(task_manager::StatTimeSigTaskParams* proto_src, stattimesigtaskparams_t* data_dst);
extern int extractPlayerTimesigTaskParams(task_manager::PlayerTimeSigTaskParams* proto_src, playertimesigtaskparams_t* data_dst);
extern int extractVibegTaskParams(task_manager::VibegTaskParams* proto_src, vibegtaskparams_t* data_dst);
extern int extractQualityTimesigTaskParams(task_manager::QualityTimeSigTaskParams* proto_src, qualitytimesigtaskparams_t* data_dst);
extern int extractSaveTimesigTaskParams(task_manager::SaveTimeSigTaskParams* proto_src, savetimesigtaskparams_t* data_dst);
extern int extractVsensorFrotTaskParams(task_manager::VsensorFrotTaskParams* proto_src, vsensorfrottaskparams_t* data_dst);
extern int extractDecimTimesigTaskParams(task_manager::DecimTimeSigTaskParams* proto_src, decimtimesigtaskparams_t* data_dst);

extern int extractStatRot(task_manager::TStatRot* proto_src, TStatRot* data_dst);

extern int packOffSet(task_manager::TOffset* proto_dst, TOffSet* data_src);
extern int packFrotLimits(task_manager::TFrotLimits* proto_dst, TFrotLimits* data_src);
extern int packStatRot(task_manager::TStatRot* proto_dst, TStatRot* data_src);
extern int packRBItem(task_manager::RBitem* proto_dst, rbitem_t* data_src);

extern int packLPFParams(task_manager::LPFilter_params* proto_dst, LPFParams_t* data_src);
extern int packBPFParams(task_manager::BPFilter_params* proto_dst, BPFParams_t* data_src);
extern int packRSMPFParams(task_manager::RSMPFilter_params* proto_dst, RSMPFParams_t* data_src);
extern int packHPFParams(task_manager::HPFilter_params* proto_dst, HPFParams_t* data_src);
extern int packSPFParams(task_manager::SPFilter_params* proto_dst, SPFParams_t* data_src);
extern int packINTGFParams(task_manager::INTGFilter_params* proto_dst, INTGFParams_t* data_src);
extern int packFParams(task_manager::FParams* proto_dst, FParams* data_src);

extern int packProtoCMN(task_manager::CommonTaskParams* proto_dst, commontaskparams_t* data_src);
extern int packVibegTaskParams(task_manager::VibegTaskParams* proto_dst, vibegtaskparams_t* data_src);

extern int packVharmonicResult(task_manager::Vharmonic_data* proto_dst, vharmonic_t* data_src);
extern int packKurtosisResult(task_manager::StatKurtosis_data* proto_dst, statkurtosis_t* data_src);
extern int packStattimesigResult(task_manager::StatTimeSig_data* proto_dst, stattimesig_t* data_src);
extern int packQualitytimesigResult(task_manager::QualityTimeSig_data* proto_dst, qualitytimesig_t* data_src);
extern int packVsensorfrotResult(task_manager::VsensorFrot_data* proto_dst, vsensorfrot_t* data_src);
extern int packDecimtimesigResult(task_manager::DecimTimeSig_data* proto_dst, decimtimesig_t* data_src);

//--------------------------------------------------------------------------------------------------------------------------------

extern int extractBatteryInformation(hardware_monitor::BatteryInformation* proto_src, t_batmon_data* data_dst);
extern int packBatteryInformation(hardware_monitor::BatteryInformation* proto_dst, t_batmon_data* data_src);

extern int extractSpectrResult(task_manager::Spectr_data* proto_src, TSimpleSpectr* class_dst); //TODO
extern int packSpectrResult(task_manager::Spectr_data* proto_dst, TSimpleSpectr* class_src);

#endif /* PROTOBUF_CONVERTORS_HPP_ */
