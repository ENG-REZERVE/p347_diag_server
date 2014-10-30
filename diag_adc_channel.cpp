#include "diag_adc_channel.hpp"

Diag_AdcChannel::Diag_AdcChannel(int index) : p347_AdcChannel(index) {
	dspemul_channel = NULL;
}

int Diag_AdcChannel::assignDSPEmul(TDSPEmul* dspemul_link) {
	if (dspemul_link != NULL) {
		dspemul_channel = dspemul_link;
		return 0;
	}
	return -EINVAL;
}

Diag_AdcChannel::~Diag_AdcChannel() {
	//destructor
}

int Diag_AdcChannel::doRetranslateSamples(unsigned long* src, int samples_cnt) {
	if (dspemul_channel != NULL) {
		return dspemul_channel->AddSig((void*)src, samples_cnt, TSDTC_24DATA_8STATUS);
	} else
		return -ENOENT;
}

TDSPEmul* Diag_AdcChannel::getEmulLink(void) {
	return dspemul_channel;
}
