#ifndef CTOOL_CONTROLLER_HPP_
#define CTOOL_CONTROLLER_HPP_

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include <string>
#include "thread.hpp"

#include "diag_adc_channel.hpp"
#include "p347_dev_helper.hpp"
#include "channel_data_structures.h"
#include "DSPEmulMCh.h"
#include "channel_manager.pb.h"

//-----------------------------------------modes

typedef boost::asio::deadline_timer		b_timer;

class ChannelManager : public p347_DevHelper<Diag_AdcChannel> {
public:
	ChannelManager();
	~ChannelManager();

	int initManager(channel_manager::ChannelManagerInitParams par);
	void exitManager();

	bool isAnyADCChannelRunning(void);
	bool isAnyRotChannelRunning(void);

	int FPGADriverReload(const std::string old_name, const std::string new_path);

	void alarmError(int error_code);
	void serverPing(int ping_code);
	boost::signals2::signal<void (int error_code)> sigAlarm;

	//DevHelper wrappers
	channel_manager::ADCTimeOffsets readADCTimeOffsets(void);
	channel_manager::RotData readRotData(unsigned char rot_idx);
	int doStartRotChannel(channel_manager::RotChannelInitParams & input_rp);
	int doSetupAdcChannel(channel_manager::ADCChannelInitParams & input_ap);
	int doStartAdcChannel(unsigned char ch_idx);
	int doStartSyncChannels(const channel_manager::SynctaskChannels & input_tsc);
	int doStopSyncChannels(const channel_manager::SynctaskChannels & input_tsc, bool waitForWrite, bool waitForRetranslate);


	//inherited publics:
	//---Freq
	//double getFreq(unsigned char rot_idx, t_rot_data* result)
	//int startRotChannel(t_rot_channel_init_params* par)
	//int stopRotChannel(unsigned char rot_idx)
	//bool isRotRunning(unsigned char rot_idx) {
	//---ADC
	//int setupAdcChannel(t_adc_channel_init_params* par)
	//int startAdcChannel(unsigned char ch_idx)
	//int stopAdcChannel(unsigned char ch_idx, bool wait_for_write, bool wait_for_retranslate)
	//int warmChannelStart(unsigned char ch_idx, unsigned short settings)
	//int warmChannelEnd(unsigned char ch_idx)
	//int startSyncChannels(t_synctask_channels *tsc)
	//int stopSyncChannels(t_synctask_channels *tsc, bool wait_for_write, bool wait_for_retranslate)
	//int readOffsets(unsigned long* dst)
	//int checkFPGAStatus(void)
	//---MUX
	//int switchCommutatorOn(void)
	//int unmuxAll(void)
	//int doMuxChannel(int ch_idx, int mux_idx)
};

#endif /* CTOOL_CONTROLLER_HPP_ */
