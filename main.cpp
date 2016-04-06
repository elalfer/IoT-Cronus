//
//  main.cpp
//  
//
//  Created by Ilya Albekht on 11/29/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

/*#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


// WolfSSL
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/test.h>*/
#include <errno.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cal.h"
#include "net.h"
#include "valve.h"
#include "gpio.h"

/*
#include <libical/ical.h>

#include "libical/icalproperty_cxx.h"
#include "libical/vcomponent_cxx.h"
#include "libical/icalrecur.h" */


// Time after which to check if any of the valves are active (sec)
#define CHECK_TIME 10
// Time to update calendar info from the internet (sec)
// #define UPDATE_TIME (60*10)
#define UPDATE_TIME (30)

//// TODO List
// * Extract cal for a week

using namespace std;
using namespace LibICal;

vector<IValveControl> g_ValveControl;

#define PIN_V1 7
#define PIN_V2 8
#define PIN_LED_ACTIVE 4

int main(int argc, char* argv[])
{

    iCalValveControl vc(GT_DEFUALT);
    GpioRelay R1(PIN_V1);

    vc.ParseICALFromFile("./valve_0.ical");
    time_t last_reload = time(0)-2*UPDATE_TIME; // Hack to force the reload on the first iteration

    while(true)
    {
        if( (time(0) - last_reload) > UPDATE_TIME )    // do it every UPDATE_TIME seconds
        {
            string ical_string("");
            string h_name("calendar.google.com");
            string URL("https://calendar.google.com/calendar/ical/04n0submlvfodumeo7ola6f90s%40group.calendar.google.com/private-b40357d4bfaee14d76ffaa65e910d554/basic.ics");
            int err;

            if( (err = load_ical_from_url(ical_string, h_name, URL, vc.LastUpdated())) == NET_SUCCESS)
            {
                vc.ParseICALFromString(ical_string);
            }
            else if(err == NET_NOT_CHANGED)
            {
                //TODO force reload from the cache once a day to generate schedule for 7 days
                //      and store it in the cache
            }
            else if(err < 0)
            {
                cerr << "ERROR: Error loading ical" << endl;
            }

            last_reload = time(0); // is it better to update the time before or after the load??
        }

        //set_valve_status(0, vc.IsActive());
        //R1.SetStatus(!R1.IsOn()); // Just swap between 2 states

        // TODO adjust sleep time for schedule update
        sleep(CHECK_TIME);
    }
    
    return 0;
}

