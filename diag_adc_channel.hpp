#ifndef _DIAG_ADC_CHANNEL_H_
#define _DIAG_ADC_CHANNEL_H_

#include "p347_adc_channel.hpp"

#include "DSPEmul.h"

class Diag_AdcChannel : public p347_AdcChannel {
public:
	Diag_AdcChannel(int index);
    ~Diag_AdcChannel(void);
    int assignDSPEmul(TDSPEmul* dspemul_link);
    TDSPEmul* getEmulLink(void);
protected:
    virtual int doRetranslateSamples(unsigned long* src, int samples_cnt);
private:
    TDSPEmul* 		dspemul_channel;
};

#endif
