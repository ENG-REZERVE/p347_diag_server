#define PROGRAM_AUTHOR  "Konstantin Utkin"
#define PROGRAM_VERSION "p347 Device Management Server " __DATE__ " " __TIME__ " by " PROGRAM_AUTHOR

/*
channels versions history:

0.11 - AvailableChannels expanded to rotchannels (23.10)
0.10 - basic working

hwmon versions history:

0.10 - basic working

*/

#define SERVER_VERSION_MAJOR		0
#define SERVER_VERSION_MINOR		11
#define SERVER_VERSION_BUILD		__DATE__ " " __TIME__

#define HWMON_VERSION_MAJOR			0
#define HWMON_VERSION_MINOR			8

#include "comm_center.hpp"
#include "hwmon_comm_center.hpp"
#include "inifile.h"

#define PRINT_USAGE		printf("USAGE EXAMPLE: ./srv_test ./srv_params.ini\n");

//---------------------------------------------------static variables
static int sping = 0;
static bool have_work = true;
static int exit_reason = END_REASON_OK;

//---------------------------------------------------signals handling
#include <execinfo.h>

void signal_handler(int sig) {
    switch (sig) {
		case SIGINT:
		case SIGTERM:
		case SIGABRT: {
			have_work = false;
			printf("Aborting program!\n");
			exit_reason = END_REASON_ABORTED_BY_USER;
		break; }
		case SIGSEGV: {
			char **strings;
			int j, nptrs;
			void *buffer[100];

			printf("calib_tool: SIGSEGV catched!\n");
			nptrs = backtrace(buffer,100);
			printf("backtrace() returned %d addresses\n", nptrs);

			strings = backtrace_symbols(buffer, nptrs);
			if (strings == NULL) {
			printf("Cannot request backtrace symbols :(");
			} else {
			for (j = 0; j < nptrs; j++)
				printf("%s\n", strings[j]);
				  free(strings);
			}

			signal(sig,SIG_DFL);
		exit(EXIT_FAILURE); }
		default: return;
    };
}




int main(int argc,char** argv) {
	printf("%s\n",PROGRAM_VERSION);

    signal(SIGINT,signal_handler);
    signal(SIGTERM,signal_handler);
    signal(SIGABRT,signal_handler);
    signal(SIGSEGV,signal_handler);

/*//test
    vharmonic_t vharmonic_result;
    task_manager::AnyTaskResult atr;
    atr.set_error_code(0);

    task_manager::Vharmonic_data* vhd = atr.mutable_harmonic_res();
    vhd->set_amp(1);
    vhd->set_ph(2);
    vhd->set_freq(3);
    vhd->set_avgcount(5);

    task_manager::TStatRot* tsr = vhd->mutable_statrot();
    tsr->set_avgcnt(1);
    tsr->set_avg(2);
    tsr->set_min(3);
    tsr->set_max(4);

    exit(0);
*/
    if (argc != 2) {
   		printf("Incorrect parameters number!\n");
   		PRINT_USAGE;
   		exit(END_REASON_INVALID_PARAMETERS_COUNT);
    }

    //starting server parameters
    char server_ipaddr[20];
    memset(&server_ipaddr[0],0,20);
    int server_port;
    int hwmon_port;
    int channels_available[p347_ADC_CHANNELS_CNT];
    int rot_available[p347_ROT_CHANNELS_CNT];

    TIniFile* ini = new TIniFile();
    if (ini == NULL) {
    	printf("Cannot create ini file class!\n");
    	exit(END_REASON_NO_MEMORY);
    }

    if (!ini->OpenIniFile(argv[1])) {
        printf("Cannot open ini file (%s)\n",argv[1]);
        delete ini;
        exit(END_REASON_NO_INI_FILE);
    }

    sprintf(&server_ipaddr[0],ini->ReadString("settings","server_ipaddr","192.168.1.197"));
    server_port = ini->ReadInt("settings","server_port",50001);
    hwmon_port = ini->ReadInt("settings","hwmon_port",50002);
    channels_available[0] = ini->ReadInt("settings","ch0",1);
    channels_available[1] = ini->ReadInt("settings","ch1",1);
    channels_available[2] = ini->ReadInt("settings","ch2",1);
    channels_available[3] = ini->ReadInt("settings","ch3",1);
    rot_available[0] = ini->ReadInt("settings","rot0",1);
    rot_available[1] = ini->ReadInt("settings","rot1",1);
    rot_available[2] = ini->ReadInt("settings","rot2",1);
    rot_available[3] = ini->ReadInt("settings","rot3",1);

    ini->CloseIniFile();
    delete ini;

    printf("LOADED SETTINGS:\n");
    printf("IP address = %s:%d/%d\n",&server_ipaddr[0],server_port,hwmon_port);
    printf("ADC Channels: [0]=%d, [1]=%d, [2]=%d, [3]=%d\n",channels_available[0],channels_available[1],channels_available[2],channels_available[3]);
    printf("ROT Channels: [0]=%d, [1]=%d, [2]=%d, [3]=%d\n",rot_available[0],rot_available[1],rot_available[2],rot_available[3]);

	CommCenter* cc = new CommCenter(&server_ipaddr[0],server_port,&channels_available[0],&rot_available[0]);
	assert(cc != NULL);

	HwMonCommCenter* hc = new HwMonCommCenter(&server_ipaddr[0],hwmon_port);
	assert(hc != NULL);

	std::string build_ver = SERVER_VERSION_BUILD;
	cc->setServerVersion(SERVER_VERSION_MAJOR,SERVER_VERSION_MINOR,build_ver);
	hc->setServerVersion(HWMON_VERSION_MAJOR,HWMON_VERSION_MINOR,build_ver);

	try {

		while(have_work == true) {
			cc->publishCast(sping);
			sping++;
			sleep(1);
		}
	}
	catch (const RCF::Exception & e) {
		printf("Server catches exception: %s\n",e.getErrorString().c_str());
	}

	delete cc;
	delete hc;

	printf("application finished\n");

	exit(exit_reason);
}
