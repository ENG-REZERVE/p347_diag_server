/*
 * hw_client_signaling.hpp
 *
 *  Created on: Oct 20, 2014
 *      Author: ukicompile
 */

#ifndef HW_CLIENT_SIGNALING_HPP_
#define HW_CLIENT_SIGNALING_HPP_

#include "hw_client.hpp"
#include <boost/signals2.hpp>
#include <boost/bind.hpp>

class p347_HwClientSignaling : public p347_HwClient {
public:
    p347_HwClientSignaling(void);
    ~p347_HwClientSignaling(void);
    void callbackBatteryData(t_batmon_data* data, int error);
    void callbackCPUTemperature(float t_min, float t_max, int error);
    void callbackPBEvent(unsigned char value, int error);

    boost::signals2::signal<void (t_batmon_data* data, int error)> sigBatteryData;
    boost::signals2::signal<void (float t_min, float t_max, int error)> sigCPUTemperature;
    boost::signals2::signal<void (unsigned char value, int error)> sigPBEvent;
};


#endif /* HW_CLIENT_SIGNALING_HPP_ */
