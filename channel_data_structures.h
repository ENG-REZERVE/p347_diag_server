#ifndef CHANNEL_DATA_STRUCTURES_H_
#define CHANNEL_DATA_STRUCTURES_H_

//#include "calib_enums.h"
#include "p347_fpga_user.h"

#define			DEFAULT_FILENAME_LEN		50

typedef struct {
	//manager logging
	int					log_level_file;
	int					log_level_console;
	int					log_level_daemon;
	//dsp helper logging
	int					dsph_level_file;
	int					dsph_level_console;
	int					dsph_level_daemon;
	//initial parameters
	int					base_timing;
	int					drv_buf_size;
	int					usr_proc_len;
	int					spi_speed_hz;
	int					main_sleep_us;
	int					idle_sleep_us;

	t_driver_timings	tdt;

	char				daemon_ipc_path[DEFAULT_FILENAME_LEN];
	int					daemon_ipc_key;

	bool				reset_at_open;
	//channel parameters
	int					chan_level_file[p347_ADC_CHANNELS_CNT];
	int					chan_level_console[p347_ADC_CHANNELS_CNT];
	int					chan_level_daemon[p347_ADC_CHANNELS_CNT];
} t_channel_manager_init_params;

#define ROT_CIP_STRUCTURE_NAME		t_rot_channel_init_params
#define ROT_CIP_PROTOCLASS_NAME		channel_manager::RotChannelInitParams

#define XFIELDS_ROTCIP \
	X(unsigned char, 	rot_idx,	"%d",	ROT_CIP_STRUCTURE_NAME,		ROT_CIP_PROTOCLASS_NAME) \
	X(unsigned short, 	av_num,		"%d",	ROT_CIP_STRUCTURE_NAME,		ROT_CIP_PROTOCLASS_NAME) \
	X(unsigned long, 	pw_min_us,	"%d",	ROT_CIP_STRUCTURE_NAME,		ROT_CIP_PROTOCLASS_NAME) \
	X(unsigned long, 	period_min_us,	"%d",	ROT_CIP_STRUCTURE_NAME,		ROT_CIP_PROTOCLASS_NAME) \
	X(unsigned long, 	period_max_us,	"%d",	ROT_CIP_STRUCTURE_NAME,		ROT_CIP_PROTOCLASS_NAME)

typedef struct {
#define X(type, name, format, stype, pclass) type name;
	XFIELDS_ROTCIP
#undef X
} ROT_CIP_STRUCTURE_NAME;

//------------------------------------------------------------------------------------------------------------

typedef struct {
	unsigned char 		ch_idx;
	unsigned char 		rot_idx;
	int 				drv_buf_size;
	int 				usr_proc_len;
	unsigned long 		usr_proc_cnt;
	int 				sen_filter_id;
	t_adc_params		ap;
} t_adc_channel_init_params;
/*
#define ADC_CIP_STRUCTURE_NAME		t_adc_channel_init_params
#define ADC_CIP_PROTOCLASS_NAME		channel_manager::ADCChannelInitParams
#define ADC_CIP_SUBSTRUCT_NAME		t_adc_params
#define ADC_CIP_SUBCLASS_NAME		channel_manager::ADCParams
#define ADC_CIP_SUBVARIABLE_NAME	ap

#define XFIELDS_ADCCIP \
	X(unsigned char,	ch_idx,			"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	X(unsigned char,	rot_idx,		"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	X(int,				drv_buf_size,	"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	X(int,				usr_proc_len,	"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	X(unsigned long,	usr_proc_cnt,	"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	X(int,				sen_filter_id,	"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	Y(ADC_CIP_SUBSTRUCT_NAME,	ADC_CIP_SUBVARIABLE_NAME,	ADC_CIP_SUBCLASS_NAME,	unsigned short,	control1,	"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	Y(ADC_CIP_SUBSTRUCT_NAME,	ADC_CIP_SUBVARIABLE_NAME,	ADC_CIP_SUBCLASS_NAME,	unsigned short,	control2,	"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	Y(ADC_CIP_SUBSTRUCT_NAME,	ADC_CIP_SUBVARIABLE_NAME,	ADC_CIP_SUBCLASS_NAME,	unsigned short,	offset,		"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	Y(ADC_CIP_SUBSTRUCT_NAME,	ADC_CIP_SUBVARIABLE_NAME,	ADC_CIP_SUBCLASS_NAME,	unsigned short,	gain,		"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	Y(ADC_CIP_SUBSTRUCT_NAME,	ADC_CIP_SUBVARIABLE_NAME,	ADC_CIP_SUBCLASS_NAME,	unsigned short,	overrange,	"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME) \
	Y(ADC_CIP_SUBSTRUCT_NAME,	ADC_CIP_SUBVARIABLE_NAME,	ADC_CIP_SUBCLASS_NAME,	unsigned short,	ch_settings,"%d",	ADC_CIP_STRUCTURE_NAME,		ADC_CIP_PROTOCLASS_NAME)

typedef struct {
#define X(type, name, format, stype, pclass) type name;
#define Y(subtype, subname, subclass, type, name, format, stype, pclass)
	XFIELDS_ADCCIP
	ADC_CIP_SUBSTRUCT_NAME ADC_CIP_SUBVARIABLE_NAME;
#undef X
} ADC_CIP_STRUCTURE_NAME;
*/
//------------------------------------------------------------------------------------------------------------

#define ROT_DATA_STRUCTURE_NAME		t_rot_data
#define ROT_DATA_PROTOCLASS_NAME	channel_manager::RotData

//type name format structure_type proto_class
#define XFIELDS_ROTDATA \
	X(bool, 			running, 	"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(unsigned short, 	pos, 		"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(unsigned long, 	min, 		"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(unsigned long, 	max, 		"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(unsigned long, 	avr, 		"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(double, 			sum, 		"%lf",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(unsigned short, 	cnt, 		"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(double, 			freq_min, 	"%lf",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(double, 			freq_max, 	"%lf",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(double, 			freq_avr, 	"%lf",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(bool, 			overhigh, 	"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME) \
	X(bool, 			underlow, 	"%d",	ROT_DATA_STRUCTURE_NAME,	ROT_DATA_PROTOCLASS_NAME)

typedef struct {
#define X(type, name, format, stype, pclass) type name;
	XFIELDS_ROTDATA
#undef X
} ROT_DATA_STRUCTURE_NAME;

extern const char* t_rot_data_names[];
extern const char* t_rot_data_typenames[];
extern const int t_rot_data_fields_number;

enum t_reflected_structures {
	TRS_ROTDATA,
	TRS_CHANNEL_MANAGER_INIT_PARAMS
};

extern int extractStructureFromClass(void* dst_structure, void* src_class, t_reflected_structures entity_type);
extern void printStructure(void* src_structure, t_reflected_structures entity_type);
extern void printClass(void* src_class, t_reflected_structures entity_type);

#endif /* CHANNEL_DATA_STRUCTURES_H_ */
