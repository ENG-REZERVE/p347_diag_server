#include "task_manager.pb.h"
#include "channel_manager.pb.h"
#include "hardware_monitor.pb.h"
#include "DSPEmulDefines.h"
#include "p347_pwr_user.h"

#include "TSpectr.h"

#include <errno.h>
#include <stdio.h>


//--------------------------------------------------------------------------------------------------------------------------------

int extractLPFilter(task_manager::LPFilter* proto_src, TLPFilter* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_index())
		data_dst->Index = proto_src->index();
	if (proto_src->has_mfreq())
		data_dst->MFreq = proto_src->mfreq();
	if (proto_src->has_rfreq())
		data_dst->RFreq = proto_src->rfreq();

	return 0;
}

int extractBPFilter(task_manager::BPFilter* proto_src, TBPFilter* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_index())
		data_dst->Index = proto_src->index();
	if (proto_src->has_cfreq())
		data_dst->CFreq = proto_src->cfreq();
	if (proto_src->has_width())
		data_dst->Width = proto_src->width();
	if (proto_src->has_oct())
		data_dst->Oct = proto_src->oct();
	if (proto_src->has_dec())
		data_dst->Dec = proto_src->dec();
	if (proto_src->has_predec())
		data_dst->PreDec = proto_src->predec();
	if (proto_src->has_bpdec())
		data_dst->BPDec = proto_src->bpdec();
	if (proto_src->has_envtype())
		data_dst->EnvType = proto_src->envtype();

	return 0;
}


int packLPFilter(task_manager::LPFilter* proto_dst, TLPFilter* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_index(data_src->Index);
	proto_dst->set_mfreq(data_src->MFreq);
	proto_dst->set_rfreq(data_src->RFreq);

	return 0;
}

int packBPFilter(task_manager::BPFilter* proto_dst, TBPFilter* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_index(data_src->Index);
	proto_dst->set_cfreq(data_src->CFreq);
	proto_dst->set_width(data_src->Width);
	proto_dst->set_oct(data_src->Oct);
	proto_dst->set_dec(data_src->Dec);
	proto_dst->set_predec(data_src->PreDec);
	proto_dst->set_bpdec(data_src->BPDec);
	proto_dst->set_envtype(data_src->EnvType);

	return 0;
}


//--------------------------------------------------------------------------------------------------------------------------------

int extractStatRot(task_manager::TStatRot* proto_src, TStatRot* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_avgcnt())
		data_dst->AvgCnt = proto_src->avgcnt();
	if (proto_src->has_avg())
		data_dst->Avg = proto_src->avg();
	if (proto_src->has_min())
		data_dst->Min = proto_src->min();
	if (proto_src->has_max())
		data_dst->Max = proto_src->max();

	return 0;
}

//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------Manual Packers
//----------------------------------------------------------------------------------------------

int packOffSet(task_manager::TOffset* proto_dst, TOffSet* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_type(data_src->Type);
	proto_dst->set_value(data_src->Value);

	return 0;
}

int packRBItem(task_manager::RBitem* proto_dst, rbitem_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_valuex(data_src->ValueY);
	proto_dst->set_valuey(data_src->ValueX);
	proto_dst->set_validextern(data_src->ValidExtern);
	proto_dst->set_valid(data_src->Valid);
	proto_dst->set_validrough(data_src->ValidRough);

    return 0;
}

int packStatRot(task_manager::TStatRot* proto_dst, TStatRot* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_avgcnt(data_src->AvgCnt);
	proto_dst->set_avg(data_src->Avg);
	proto_dst->set_min(data_src->Min);
	proto_dst->set_max(data_src->Max);

	return 0;
}

int packLPFParams(task_manager::LPFilter_params* proto_dst, LPFParams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_lpfilterindex(data_src->LPFilterIndex);
	proto_dst->set_lpfilterdecim(data_src->LPFilterDecim);
	proto_dst->set_lpfiltertype(data_src->LPFilterType);
	proto_dst->set_lpfilterusualw(data_src->LPFilterUsualW);

	return 0;
}

int packBPFParams(task_manager::BPFilter_params* proto_dst, BPFParams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_bpfilterindex(data_src->BPFilterIndex);
	proto_dst->set_bpfiltertype(data_src->BPFilterType);
	proto_dst->set_bpfilterusualw(data_src->BPFilterUsualW);

	return 0;
}

int packRSMPFParams(task_manager::RSMPFilter_params* proto_dst, RSMPFParams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_rsmpfilterindex(data_src->RSMPFilterIndex);
	proto_dst->set_rsmpfiltertype(data_src->RSMPFilterType);
	proto_dst->set_rsmpfilterusualw(data_src->RSMPFilterUsualW);

	return 0;
}

int packHPFParams(task_manager::HPFilter_params* proto_dst, HPFParams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_hpfilterindex(data_src->HPFilterIndex);
	proto_dst->set_hpfiltertype(data_src->HPFilterType);
	proto_dst->set_hpfilterparam1(data_src->HPFilterParam1);

	return 0;
}

int packSPFParams(task_manager::SPFilter_params* proto_dst, SPFParams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_spfilterindex(data_src->SPFilterIndex);
	proto_dst->set_spfiltertype(data_src->SPFilterType);
	proto_dst->set_spfilterusualw(data_src->SPFilterUsualW);

	return 0;
}

int packINTGFParams(task_manager::INTGFilter_params* proto_dst, INTGFParams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_intgfilterindex(data_src->INTGFilterIndex);
	proto_dst->set_intgfiltertype(data_src->INTGFilterType);

	return 0;
}

int packFParams(task_manager::FParams* proto_dst, FParams* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_type(data_src->Type);
	switch (data_src->Type) {
		case FPARAMS_LP: {
			task_manager::LPFilter_params* lpf = proto_dst->mutable_lpf();
			packLPFParams(lpf,&data_src->LPF);
		break; }
		case FPARAMS_BP: {
			task_manager::BPFilter_params* bpf = proto_dst->mutable_bpf();
			packBPFParams(bpf,&data_src->BPF);
		break; }
		case FPARAMS_RSMP: {
			task_manager::RSMPFilter_params* rsmpf = proto_dst->mutable_rsmpf();
			packRSMPFParams(rsmpf,&data_src->RSMPF);
		break; }
		case FPARAMS_HP: {
			task_manager::HPFilter_params* hpf = proto_dst->mutable_hpf();
			packHPFParams(hpf,&data_src->HPF);
		break; }
		case FPARAMS_SP: {
			task_manager::SPFilter_params* spf = proto_dst->mutable_spf();
			packSPFParams(spf,&data_src->SPF);
		break; }
		case FPARAMS_INTG: {
			task_manager::INTGFilter_params* intgf = proto_dst->mutable_intgf();
			packINTGFParams(intgf,&data_src->INTGF);
		break; }
		default: {
			proto_dst->set_type(FPARAMS_NONE);
		break; }
	}

	return 0;
}

int packFrotLimits(task_manager::TFrotLimits* proto_dst, TFrotLimits* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_frot_min(data_src->Frot_min);
	proto_dst->set_frot_max(data_src->Frot_max);
	proto_dst->set_deltafrotonemeas(data_src->DeltaFrotOneMeas);
	proto_dst->set_deltafrotallmeas(data_src->DeltaFrotAllMeas);

	return 0;
}

int packProtoCMN(task_manager::CommonTaskParams* proto_dst, commontaskparams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	task_manager::TOffset* off_ptr = proto_dst->mutable_offset();
	TOffSet* data_ptr = &data_src->OffSet;
	packOffSet(off_ptr,data_ptr);

	off_ptr = proto_dst->mutable_deltaoffset();
	data_ptr = &data_src->DeltaOffSet;
	packOffSet(off_ptr,data_ptr);

	proto_dst->set_monitoring(data_src->Monitoring);
	proto_dst->set_timesigtype(data_src->TimeSigType);
	proto_dst->set_lpfilterindex(data_src->LPFilterIndex);
	proto_dst->set_lpfilterdecim(data_src->LPFilterDecim);
	proto_dst->set_lpfiltertype(data_src->LPFilterType);
	proto_dst->set_lpfilterusualw(data_src->LPFilterUsualW);
	proto_dst->set_bpfilterindex(data_src->BPFilterIndex);
	proto_dst->set_bpfiltertype(data_src->BPFilterType);
	proto_dst->set_bpfilterusualw(data_src->BPFilterUsualW);
	proto_dst->set_rsmpfilterindex(data_src->RSMPFilterIndex);
	proto_dst->set_rsmpfiltertype(data_src->RSMPFilterType);
	proto_dst->set_rsmpfilterusualw(data_src->RSMPFilterUsualW);
	proto_dst->set_hpfilterindex(data_src->HPFilterIndex);
	proto_dst->set_hpfiltertype(data_src->HPFilterType);
	proto_dst->set_hpfilterparam1(data_src->HPFilterParam1);

	int sz = data_src->FParamsCount;
	for (int i=0; i < sz; i++) {
		task_manager::FParams* fpl = proto_dst->add_fparamslist();
		FParams* fp = &data_src->FParamsList[i];
		packFParams(fpl,fp);
	}

	proto_dst->set_fparamscount(sz);
	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::TFrotLimits* fr = proto_dst->mutable_frotlimits();
	packFrotLimits(fr,&data_src->FrotLimits);

	proto_dst->set_frotlimitsactive(data_src->FrotLimitsActive);
	proto_dst->set_stabcontrol(data_src->StabControl);
	proto_dst->set_rotcontrol(data_src->RotControl);
	proto_dst->set_waitperiodforrotmetka(data_src->WaitPeriodForRotMetka);
	proto_dst->set_timesigsrctype(data_src->TimeSigSrcType);
	proto_dst->set_timesigid(data_src->TimeSigID);
	proto_dst->set_inittaskstate(data_src->InitTaskState);
	proto_dst->set_finaltaskstate(data_src->FinalTaskState);
	proto_dst->set_tsdtc(data_src->TSDTC);
	proto_dst->set_calctypetsdtc(data_src->CalcTypeTSDTC);
	proto_dst->set_tde(data_src->TDE);

	return 0;
}

int packVibegTaskParams(task_manager::VibegTaskParams* proto_dst, vibegtaskparams_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_frot_base(data_src->Frot_base);
	proto_dst->set_frotavgmetkascount(data_src->FrotAvgMetkasCount);

	task_manager::CommonTaskParams* ctp = proto_dst->mutable_cmn();
	commontaskparams_t* cmnp = &data_src->CMN;

	return packProtoCMN(ctp,cmnp);
}

int packVharmonicResult(task_manager::Vharmonic_data* proto_dst, vharmonic_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_amp(data_src->amp);
	proto_dst->set_ph(data_src->ph);
	proto_dst->set_freq(data_src->freq);
	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::TStatRot* sr = proto_dst->mutable_statrot();
	TStatRot* tsr = &data_src->StatRot;

	return packStatRot(sr,tsr);
}

int packKurtosisResult(task_manager::StatKurtosis_data* proto_dst, statkurtosis_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_min(data_src->min);
	proto_dst->set_max(data_src->max);
	proto_dst->set_mean(data_src->mean);
	proto_dst->set_std(data_src->std);
	proto_dst->set_kurtosis(data_src->kurtosis);
	proto_dst->set_peak_factor(data_src->peak_factor);
	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::TStatRot* sr = proto_dst->mutable_statrot();
	TStatRot* tsr = &data_src->StatRot;

	return packStatRot(sr,tsr);
}

int packStattimesigResult(task_manager::StatTimeSig_data* proto_dst, stattimesig_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_min(data_src->min);
	proto_dst->set_max(data_src->max);
	proto_dst->set_mean(data_src->mean);
	proto_dst->set_std(data_src->std);
	proto_dst->set_kurtosis(data_src->kurtosis);
	proto_dst->set_peak_factor(data_src->peak_factor);
	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::TStatRot* sr = proto_dst->mutable_statrot();
	TStatRot* tsr = &data_src->StatRot;

	return packStatRot(sr,tsr);
}

int packQualitytimesigResult(task_manager::QualityTimeSig_data* proto_dst, qualitytimesig_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_sigoverflow(data_src->SigOverFlow);
	proto_dst->set_sigtoolow(data_src->SigTooLow);
	proto_dst->set_sigtoohigh(data_src->SigTooHigh);
	proto_dst->set_amppeaktopeakcalc(data_src->AmpPeakToPeakCalc);
	proto_dst->set_ampstdcalc(data_src->AmpStdCalc);
	proto_dst->set_sigoverflowcount(data_src->SigOverFlowCount);
	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::TStatRot* sr = proto_dst->mutable_statrot();
	TStatRot* tsr = &data_src->StatRot;

	return packStatRot(sr,tsr);
}

int packSavetimesigResult(task_manager::SaveTimeSig_data* proto_dst, savetimesig_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::TStatRot* sr = proto_dst->mutable_statrot();
	TStatRot* tsr = &data_src->StatRot;

	return packStatRot(sr,tsr);
}

int packVsensorfrotResult(task_manager::VsensorFrot_data* proto_dst, vsensorfrot_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::RBitem* rbi = proto_dst->mutable_frot();
	rbitem_t* rbit = &data_src->Frot;
	packRBItem(rbi,rbit);

	task_manager::TStatRot* sr = proto_dst->mutable_statrot();
	TStatRot* tsr = &data_src->StatRot;
	return packStatRot(sr,tsr);
}

int packDecimtimesigResult(task_manager::DecimTimeSig_data* proto_dst, decimtimesig_t* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_avgcount(data_src->AvgCount);

	task_manager::TStatRot* sr = proto_dst->mutable_statrot();
	TStatRot* tsr = &data_src->StatRot;
	return packStatRot(sr,tsr);
}

//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------Manual Extractors
//----------------------------------------------------------------------------------------------

int extractLPFilterParams(task_manager::LPFilter_params* proto_src, LPFParams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_lpfilterindex())
		data_dst->LPFilterIndex = proto_src->lpfilterindex();
	if (proto_src->has_lpfilterdecim())
		data_dst->LPFilterDecim = proto_src->lpfilterdecim();
	if (proto_src->has_lpfiltertype())
		data_dst->LPFilterType = proto_src->lpfiltertype();
	if (proto_src->has_lpfilterusualw())
		data_dst->LPFilterUsualW = proto_src->lpfilterusualw();

	return 0;
}

int extractBPFilterParams(task_manager::BPFilter_params* proto_src, BPFParams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_bpfilterindex())
		data_dst->BPFilterIndex = proto_src->bpfilterindex();
	if (proto_src->has_bpfiltertype())
		data_dst->BPFilterType = proto_src->bpfiltertype();
	if (proto_src->has_bpfilterusualw())
		data_dst->BPFilterUsualW = proto_src->bpfilterusualw();

	return 0;
}

int extractRSMPFilterParams(task_manager::RSMPFilter_params* proto_src, RSMPFParams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_rsmpfilterindex())
		data_dst->RSMPFilterIndex = proto_src->rsmpfilterindex();
	if (proto_src->has_rsmpfiltertype())
		data_dst->RSMPFilterType = proto_src->rsmpfiltertype();
	if (proto_src->has_rsmpfilterusualw())
		data_dst->RSMPFilterUsualW = proto_src->rsmpfilterusualw();

	return 0;
}

int extractHPFilterParams(task_manager::HPFilter_params* proto_src, HPFParams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_hpfilterindex())
		data_dst->HPFilterIndex = proto_src->hpfilterindex();
	if (proto_src->has_hpfiltertype())
		data_dst->HPFilterType = proto_src->hpfiltertype();
	if (proto_src->has_hpfilterparam1())
		data_dst->HPFilterParam1 = proto_src->hpfilterparam1();

	return 0;
}

int extractSPFilterParams(task_manager::SPFilter_params* proto_src, SPFParams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_spfilterindex())
		data_dst->SPFilterIndex = proto_src->spfilterindex();
	if (proto_src->has_spfiltertype())
		data_dst->SPFilterType = proto_src->spfiltertype();
	if (proto_src->has_spfilterusualw())
		data_dst->SPFilterUsualW = proto_src->spfilterusualw();

	return 0;
}

int extractINTGFilterParams(task_manager::INTGFilter_params* proto_src, INTGFParams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_intgfilterindex())
		data_dst->INTGFilterIndex = proto_src->intgfilterindex();
	if (proto_src->has_intgfiltertype())
		data_dst->INTGFilterType = proto_src->intgfiltertype();

	return 0;
}

int extractProtoCMN(task_manager::CommonTaskParams* proto_src, commontaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	if (proto_src->has_offset()) {
		task_manager::TOffset* toff = proto_src->mutable_offset();
		if (toff->has_type())
			data_dst->OffSet.Type = toff->type();
		if (toff->has_value())
			data_dst->OffSet.Value = toff->value();
	}
	if (proto_src->has_deltaoffset()) {
		task_manager::TOffset* toff = proto_src->mutable_deltaoffset();
		if (toff->has_type())
			data_dst->DeltaOffSet.Type = toff->type();
		if (toff->has_value())
			data_dst->DeltaOffSet.Value = toff->value();
	}

	if (proto_src->has_monitoring()) {
		data_dst->Monitoring = proto_src->monitoring();
	}
	if (proto_src->has_timesigtype()) {
		data_dst->TimeSigType = proto_src->timesigtype();
	}
	if (proto_src->has_lpfilterindex()) {
		data_dst->LPFilterIndex = proto_src->lpfilterindex();
	}
	if (proto_src->has_lpfilterdecim()) {
		data_dst->LPFilterDecim = proto_src->lpfilterdecim();
	}
	if (proto_src->has_lpfiltertype()) {
		data_dst->LPFilterType = proto_src->lpfiltertype();
	}
	if (proto_src->has_lpfilterusualw()) {
		data_dst->LPFilterUsualW = proto_src->lpfilterusualw();
	}
	if (proto_src->has_bpfilterindex()) {
		data_dst->BPFilterIndex = proto_src->bpfilterindex();
	}
	if (proto_src->has_bpfiltertype()) {
		data_dst->BPFilterType = proto_src->bpfiltertype();
	}
	if (proto_src->has_bpfilterusualw()) {
		data_dst->BPFilterUsualW = proto_src->bpfilterusualw();
	}
	if (proto_src->has_bpfilterindex()) {
		data_dst->BPFilterIndex = proto_src->bpfilterindex();
	}
	if (proto_src->has_bpfiltertype()) {
		data_dst->BPFilterType = proto_src->bpfiltertype();
	}
	if (proto_src->has_bpfilterusualw()) {
		data_dst->BPFilterUsualW = proto_src->bpfilterusualw();
	}
	if (proto_src->has_rsmpfilterindex()) {
		data_dst->RSMPFilterIndex = proto_src->rsmpfilterindex();
	}
	if (proto_src->has_rsmpfiltertype()) {
		data_dst->RSMPFilterType = proto_src->rsmpfiltertype();
	}
	if (proto_src->has_rsmpfilterusualw()) {
		data_dst->RSMPFilterUsualW = proto_src->rsmpfilterusualw();
	}
	if (proto_src->has_hpfilterindex()) {
		data_dst->HPFilterIndex = proto_src->hpfilterindex();
	}
	if (proto_src->has_hpfiltertype()) {
		data_dst->HPFilterType = proto_src->hpfiltertype();
	}
	if (proto_src->has_hpfilterparam1()) {
		data_dst->HPFilterParam1 = proto_src->hpfilterparam1();
	}
	if (proto_src->has_fparamscount()) {
		data_dst->FParamsCount = proto_src->fparamscount();
	}

	if (proto_src->fparamslist_size() > data_dst->FParamsCount)
		data_dst->FParamsCount = proto_src->fparamslist_size();
	if (data_dst->FParamsCount > FPARAMSLIST_MAX_COUNT)
		data_dst->FParamsCount = FPARAMSLIST_MAX_COUNT;

	if (data_dst->FParamsCount > 0) for (int i=0; i<data_dst->FParamsCount; i++) {
		//task_manager::FParamsList* fpl = mutable_fparamslist(i);
		FParams* data_ptr = &data_dst->FParamsList[i];
		task_manager::FParams* fpl = proto_src->mutable_fparamslist(i);

		if (fpl->has_type())
			data_ptr->Type = fpl->type();

		if (fpl->has_lpf()) {
			task_manager::LPFilter_params* lp = fpl->mutable_lpf();
			extractLPFilterParams(lp, (LPFParams_t*)data_ptr);
			continue; //we must go to next iteration, because FParams is an union of types
		}
		if (fpl->has_bpf()) {
			task_manager::BPFilter_params* bp = fpl->mutable_bpf();
			extractBPFilterParams(bp, (BPFParams_t*)data_ptr);
			continue;
		}
		if (fpl->has_rsmpf()) {
			task_manager::RSMPFilter_params* rsmp = fpl->mutable_rsmpf();
			extractRSMPFilterParams(rsmp, (RSMPFParams_t*)data_ptr);
			continue;
		}
		if (fpl->has_hpf()) {
			task_manager::HPFilter_params* hp = fpl->mutable_hpf();
			extractHPFilterParams(hp, (HPFParams_t*)data_ptr);
			continue;
		}
		if (fpl->has_spf()) {
			task_manager::SPFilter_params* sp = fpl->mutable_spf();
			extractSPFilterParams(sp, (SPFParams_t*)data_ptr);
			continue;
		}
		if (fpl->has_intgf()) {
			task_manager::INTGFilter_params* intg = fpl->mutable_intgf();
			extractINTGFilterParams(intg, (INTGFParams_t*)data_ptr);
			continue;
		}
	}

	if (proto_src->has_avgcount()) {
		data_dst->AvgCount = proto_src->avgcount();
	}
	if (proto_src->has_frotlimits()) {
		task_manager::TFrotLimits* tfrot = proto_src->mutable_frotlimits();
		if (tfrot->has_frot_min())
			data_dst->FrotLimits.Frot_min = tfrot->frot_min();
		if (tfrot->has_frot_max())
			data_dst->FrotLimits.Frot_max = tfrot->frot_max();
		if (tfrot->has_deltafrotonemeas())
			data_dst->FrotLimits.DeltaFrotOneMeas = tfrot->deltafrotonemeas();
		if (tfrot->has_deltafrotallmeas())
			data_dst->FrotLimits.DeltaFrotAllMeas = tfrot->deltafrotallmeas();
	}
	if (proto_src->has_frotlimitsactive()) {
		data_dst->FrotLimitsActive = proto_src->frotlimitsactive();
	}
	if (proto_src->has_stabcontrol()) {
		data_dst->StabControl = proto_src->stabcontrol();
	}
	if (proto_src->has_rotcontrol()) {
		data_dst->RotControl = proto_src->rotcontrol();
	}
	if (proto_src->has_waitperiodforrotmetka()) {
		data_dst->WaitPeriodForRotMetka = proto_src->waitperiodforrotmetka();
	}
	if (proto_src->has_timesigsrctype()) {
		data_dst->TimeSigSrcType = proto_src->timesigsrctype();
	}
	if (proto_src->has_timesigid()) {
		data_dst->TimeSigID = proto_src->timesigid();
	}
	if (proto_src->has_inittaskstate()) {
		data_dst->InitTaskState = proto_src->inittaskstate();
	}
	if (proto_src->has_finaltaskstate()) {
		data_dst->FinalTaskState = proto_src->finaltaskstate();
	}
	if (proto_src->has_tsdtc()) {
		data_dst->TSDTC = proto_src->tsdtc();
	}
	if (proto_src->has_calctypetsdtc()) {
		data_dst->CalcTypeTSDTC = proto_src->calctypetsdtc();
	}
	if (proto_src->has_tde()) {
		data_dst->TDE = proto_src->tde();
	}

	return 0;
}



int extractSpectrTaskParams(task_manager::SpectrTaskParams* proto_src, spectrtaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	if (proto_src->has_spectrclasscode()) {
		data_dst->SpectrClassCode = proto_src->spectrclasscode();
	}
	if (proto_src->has_spectlinesarrindex()) {
		data_dst->SpectLinesArrIndex = proto_src->spectlinesarrindex();
	}
	if (proto_src->has_spectampunit()) {
		data_dst->SpectAmpUnit = proto_src->spectampunit();
	}
	if (proto_src->has_removeavgline()) {
		data_dst->RemoveAvgLine = proto_src->removeavgline();
	}

	return 0;
}

int extractVharmonicTaskParams(task_manager::VharmonicTaskParams* proto_src, vharmonicstaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	if (proto_src->has_vharmonicscount()) {
		data_dst->VHarmonicsCount = proto_src->vharmonicscount();
	}
	if (proto_src->has_vharmonicsavgcount()) {
		data_dst->VHarmonicsAvgCount = proto_src->vharmonicsavgcount();
	}
	if (proto_src->has_spectlinesarrindex()) {
		data_dst->SpectLinesArrIndex = proto_src->spectlinesarrindex();
	}

	return 0;
}

int extractStatKurtosisTaskParams(task_manager::StatKurtosisTaskParams* proto_src, statkurtosistaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	if (proto_src->has_onemeastimesiglength()) {
		data_dst->OneMeasTimeSigLength = proto_src->onemeastimesiglength();
	}
	if (proto_src->has_meascount()) {
		data_dst->MeasCount = proto_src->meascount();
	}
	if (proto_src->has_coefcover()) {
		data_dst->CoefCover = proto_src->coefcover();
	}

	return 0;
}

int extractStatTimesigTaskParams(task_manager::StatTimeSigTaskParams* proto_src, stattimesigtaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	if (proto_src->has_onemeastimesiglength()) {
		data_dst->OneMeasTimeSigLength = proto_src->onemeastimesiglength();
	}
	if (proto_src->has_ampunit()) {
		data_dst->AmpUnit = proto_src->ampunit();
	}
	if (proto_src->has_integrateintimedomain()) {
		data_dst->IntegrateInTimeDomain = proto_src->integrateintimedomain();
	}

	return 0;
}

int extractPlayerTimesigTaskParams(task_manager::PlayerTimeSigTaskParams* proto_src, playertimesigtaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	return 0;
}

int extractVibegTaskParams(task_manager::VibegTaskParams* proto_src, vibegtaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	if (proto_src->has_frot_base()) {
		data_dst->Frot_base = proto_src->frot_base();
	}
	if (proto_src->has_frotavgmetkascount()) {
		data_dst->FrotAvgMetkasCount = proto_src->frotavgmetkascount();
	}

	return extractProtoCMN(ctp,cmnp);
}

int extractQualityTimesigTaskParams(task_manager::QualityTimeSigTaskParams* proto_src, qualitytimesigtaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	if (proto_src->has_onemeastimesiglength()) {
		data_dst->OneMeasTimeSigLength = proto_src->onemeastimesiglength();
	}
	if (proto_src->has_amppeaktopeakmin()) {
		data_dst->AmpPeakToPeakMin = proto_src->amppeaktopeakmin();
	}
	if (proto_src->has_ampstdmin()) {
		data_dst->AmpStdMin = proto_src->ampstdmin();
	}
	if (proto_src->has_ampstdmax()) {
		data_dst->AmpStdMax = proto_src->ampstdmax();
	}

	return 0;
}

int extractSaveTimesigTaskParams(task_manager::SaveTimeSigTaskParams* proto_src, savetimesigtaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	if (proto_src->has_filename()) {
		memset(&data_dst->FileName[0],0,MAX_FILENAMEFORSAVE_LENGTH);
		sprintf(&data_dst->FileName[0],"%s",proto_src->filename().c_str());
		//data_dst->FileName.assign(proto_src->filename());
	}
	if (proto_src->has_oneparttimesiglength()) {
		data_dst->OnePartTimeSigLength = proto_src->oneparttimesiglength();
	}
	if (proto_src->has_totaltimesiglength()) {
		data_dst->TotalTimeSigLength = proto_src->totaltimesiglength();
	}
	if (proto_src->has_siglengthwithfssrc()) {
		data_dst->SigLengthWithFsSrc = proto_src->siglengthwithfssrc();
	}
	if (proto_src->has_tsdtc()) {
		data_dst->TSDTC = proto_src->tsdtc();
	}

	return 0;
}

int extractVsensorFrotTaskParams(task_manager::VsensorFrotTaskParams* proto_src, vsensorfrottaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	//task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	//commontaskparams_t* cmnp = &data_dst->CMN;
	//extractProtoCMN(ctp,cmnp);
	task_manager::SpectrTaskParams* stp = proto_src->mutable_stp();
	spectrtaskparams_t* sp = &data_dst->STP;
	extractSpectrTaskParams(stp,sp);

	if (proto_src->has_deltaoffsetinsec()) {
		data_dst->DeltaOffSetInSec = proto_src->deltaoffsetinsec();
	}
	if (proto_src->has_scalekoefffft()) {
		data_dst->ScaleKoeffFFT = proto_src->scalekoefffft();
	}
	if (proto_src->has_frotmin()) {
		data_dst->FrotMin = proto_src->frotmin();
	}
	if (proto_src->has_frotmax()) {
		data_dst->FrotMax = proto_src->frotmax();
	}
	if (proto_src->has_frotaccelmax()) {
		data_dst->FrotAccelMax = proto_src->frotaccelmax();
	}
	if (proto_src->has_ampmodulmin()) {
		data_dst->AmpModulMin = proto_src->ampmodulmin();
	}
	if (proto_src->has_stdmin()) {
		data_dst->STDMin = proto_src->stdmin();
	}
	if (proto_src->has_rbfilterlength()) {
		data_dst->RBFilterLength = proto_src->rbfilterlength();
	}

	return 0;
}

int extractDecimTimesigTaskParams(task_manager::DecimTimeSigTaskParams* proto_src, decimtimesigtaskparams_t* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;

	task_manager::CommonTaskParams* ctp = proto_src->mutable_cmn();
	commontaskparams_t* cmnp = &data_dst->CMN;

	extractProtoCMN(ctp,cmnp);

	if (proto_src->has_oneparttimesiglength()) {
		data_dst->OnePartTimeSigLength = proto_src->oneparttimesiglength();
	}
	if (proto_src->has_forgetfssrc()) {
		data_dst->ForgetFsSrc = proto_src->forgetfssrc();
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------HWMON

int extractBatteryInformation(hardware_monitor::BatteryInformation* proto_src, t_batmon_data* data_dst) {
	if ((proto_src == NULL) || (data_dst == NULL)) return -EINVAL;
	if (proto_src->has_is_bm_connected()) {
		data_dst->is_bm_connected=proto_src->is_bm_connected();
	}
	if (proto_src->has_is_charging()) {
		data_dst->is_charging=proto_src->is_charging();
	}
	if (proto_src->has_is_fault()) {
		data_dst->is_fault=proto_src->is_fault();
	}
	if (proto_src->has_shdn_state()) {
		data_dst->shdn_state=proto_src->shdn_state();
	}
	if (proto_src->has_battery_presense()) {
		data_dst->battery_presense=proto_src->battery_presense();
	}
	if (proto_src->has_outer_power_presense()) {
		data_dst->outer_power_presense=proto_src->outer_power_presense();
	}
	if (proto_src->has_alarm_low_voltage()) {
		data_dst->alarm_low_voltage=proto_src->alarm_low_voltage();
	}
	if (proto_src->has_no_connect_counter()) {
		data_dst->no_connect_counter=proto_src->no_connect_counter();
	}
	if (proto_src->has_val_base_charge_uah()) {
		data_dst->val_base_charge_uAh=proto_src->val_base_charge_uah();
	}
	if (proto_src->has_val_current_charge_uah()) {
		data_dst->val_current_charge_uAh=proto_src->val_current_charge_uah();
	}
	if (proto_src->has_val_icharge_ua()) {
		data_dst->val_icharge_uA=proto_src->val_icharge_ua();
	}
	if (proto_src->has_val_ucharge_mv()) {
		data_dst->val_ucharge_mV=proto_src->val_ucharge_mv();
	}
	if (proto_src->has_fullchg_bit()) {
		data_dst->fullchg_bit=proto_src->fullchg_bit();
	}
	if (proto_src->has_fastchg_bit()) {
		data_dst->fastchg_bit=proto_src->fastchg_bit();
	}
	if (proto_src->has_hex_current_charge()) {
		data_dst->hex_current_charge=proto_src->hex_current_charge();
	}
	if (proto_src->has_charge_percent()) {
		data_dst->charge_percent=proto_src->charge_percent();
	}
	if (proto_src->has_minutes_left()) {
		data_dst->minutes_left=proto_src->minutes_left();
	}
	return 0;
}

int packBatteryInformation(hardware_monitor::BatteryInformation* proto_dst, t_batmon_data* data_src) {
	if ((proto_dst == NULL) || (data_src == NULL)) return -EINVAL;

	proto_dst->set_is_bm_connected(data_src->is_bm_connected);
	proto_dst->set_is_charging(data_src->is_charging);
	proto_dst->set_is_fault(data_src->is_fault);
	proto_dst->set_shdn_state(data_src->shdn_state);
	proto_dst->set_battery_presense(data_src->battery_presense);
	proto_dst->set_outer_power_presense(data_src->outer_power_presense);
	proto_dst->set_alarm_low_voltage(data_src->alarm_low_voltage);
	proto_dst->set_no_connect_counter(data_src->no_connect_counter);
	proto_dst->set_val_base_charge_uah(data_src->val_base_charge_uAh);
	proto_dst->set_val_current_charge_uah(data_src->val_current_charge_uAh);
	proto_dst->set_val_icharge_ua(data_src->val_icharge_uA);
	proto_dst->set_val_ucharge_mv(data_src->val_ucharge_mV);
	proto_dst->set_fullchg_bit(data_src->fullchg_bit);
	proto_dst->set_fastchg_bit(data_src->fastchg_bit);
	proto_dst->set_hex_current_charge(data_src->hex_current_charge);
	proto_dst->set_charge_percent(data_src->charge_percent);
	proto_dst->set_minutes_left(data_src->minutes_left);

	return 0;
}

//-----------------------------------------------------------------------------------------classes

int extractSpectrResult(task_manager::Spectr_data* proto_src, TSimpleSpectr* class_dst) {
	if ((proto_src == NULL) || (class_dst == NULL)) return -EINVAL;


	spectrtaskparams_t spt;
	spt.SpectrClassCode = TSC_SIMPLE;
	if (proto_src->has_ftype())
		spt.SpectrType = proto_src->ftype();

	task_manager::LPFilter* lpf = proto_src->mutable_flpfilter();
	//TLPFilter tlpf;
	//extractLPFilter(lpf,&tlpf);
	spt.LPFilterIndex = lpf->index();

	task_manager::BPFilter* bpf = proto_src->mutable_fbpfilter();
	//TLPFilter tbpf;
	//extractBPFilter(bpf,&tbpf);
	spt.BPFilterIndex = bpf->index();

	spt.SpectLinesArrIndex = GetSpectLinesArIndexByLinesCount(proto_src->flinescnt());

	if (class_dst->FillParams(spt) == false) return -EINVAL;

	/*
	task_manager::TStatRot* tsr = proto_src->mutable_fstatrot();
	TStatRot tsrc;
	extractStatRot(tsr,&tsrc);
	*/

	if (proto_src->has_favgcount())
		class_dst->SetAvgCount(proto_src->favgcount());
	//if (proto_src->has_fbegfreq())
	//	class_dst->SetBegFreq(proto_src->fbegfreq());
	//if (proto_src->has_ffreqstep())
	//	class_dst->SetFreqStep(proto_src->ffreqstep());

	unsigned short amp_unit = 999;
	if (proto_src->has_fampunit())
		amp_unit = proto_src->fampunit();

	if (GetAmpUnitValid(amp_unit)) {
		TDSPE_Float* data = NULL;
		int len;

		if (amp_unit == TU_AMP_A_M_S2) {
			len = proto_src->fsiamp_size();
			if (len > 0) {
				data = new TDSPE_Float[len];
				for (int i=0; i<len; i++) {
					data[i] = proto_src->fsiamp(i);
				}
			}
			//Cannot use direct copy: TDSPE_Float can be in another format
			/*
			RepeatedField< double >* rf = proto_src->mutable_fsiamp();
			if ((len > 0) && (rf != NULL)) {
				data = rf->data();
				if (data != NULL) {
					SetAmp(data, len, amp_unit);
				}
			}
			*/
		} else {
			len = proto_src->famp_size();
			if (len > 0) {
				data = new TDSPE_Float[len];
				for (int i=0; i<len; i++) {
					data[i] = proto_src->famp(i);
				}
			}
		}

		if (data != NULL) {
			class_dst->SetAmp(data, len, amp_unit);
			delete[] data;
		}
	}

	//TODO: verify re-calculated fields

	return 0;
}

int packSpectrResult(task_manager::Spectr_data* proto_dst, TSimpleSpectr* class_src) {
	if ((proto_dst == NULL) || (class_src == NULL)) return -EINVAL;

	unsigned short amp_unit = class_src->GetAmpUnit();
	if (!GetAmpUnitValid(amp_unit)) return -EINVAL;

	proto_dst->set_ftype(class_src->GetType());

	task_manager::LPFilter* lpf = proto_dst->mutable_flpfilter();
	TLPFilter* tlpf = class_src->GetLPFilter();
    packLPFilter(lpf,tlpf);

    task_manager::BPFilter* bpf = proto_dst->mutable_fbpfilter();
    TBPFilter* tbpf = class_src->GetBPFilter();
    packBPFilter(bpf,tbpf);

    task_manager::TStatRot* tsr = proto_dst->mutable_fstatrot();
    TStatRot* tsrc = class_src->GetStatRot();
    packStatRot(tsr,tsrc);

    proto_dst->set_favgcount(class_src->GetAvgCount());
    proto_dst->set_fbegfreq(class_src->GetBegFreq());
    proto_dst->set_ffreqstep(class_src->GetFreqStep());

    proto_dst->set_fampunit(amp_unit);

    int len = class_src->GetLinesCnt();
    proto_dst->set_flinescnt(len);

    if (amp_unit == TU_AMP_A_M_S2) {
    	TDSPE_Float* data = class_src->GetSIAmp();
        if (data != NULL)
        	for (int i=0; i<len; i++)
        		proto_dst->add_fsiamp(data[i]);
    } else {
		TDSPE_Float* data = class_src->GetAmp();
		if (data != NULL)
			for (int i=0; i<len; i++)
				proto_dst->add_famp(data[i]);
    }

	return 0;
}
