// Copyright 2016 Ilya Albrekht
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "cal.h"
#include "net.h"
#include "channel.h"
#include "gpio.h"

#include "log.h"

#include <signal.h>

#include "ArduinoJson/ArduinoJson.hpp"


/*
#include <libical/ical.h>

#include "libical/icalproperty_cxx.h"
#include "libical/vcomponent_cxx.h"
#include "libical/icalrecur.h" */


// Time after which to check if any of the valves are active (sec)
#define CHECK_TIME 10
// Time to update calendar info from the internet (sec)
#define UPDATE_TIME (60*60)

//// TODO List
// * Extract cal for a week

using namespace std;
using namespace LibICal;

// Global variables
int g_DebugLevel=6;
std::vector<shared_ptr<channel_t>> valves;

// Signal handlers
void sighandler_stop(int id)
{
    for(auto &v: valves)
        v.reset();
    exit(0);
}

#define PIN_LED_ACTIVE 4

void ParseOptions(int argc, char* argv[])
{
    int i=1;
    for(; i<argc; i++)
    {
        if(strcmp("--debug", argv[i]) == 0)
        {
            g_DebugLevel = 7;
            DEBUG_PRINT(LOG_INFO, "Debug print enabled");
        }

        // Run channel <CH> for <N> minutes
        // must be last one in the list
        if(strcmp("--run", argv[i]) == 0)
        {
            if( argc < i+2 )
            {
                cerr << "Invalid number of arguments\n";
                exit(-1);
            }

            int ch = atoi( argv[i+1] );
            int t  = atoi( argv[i+2] );

            DEBUG_PRINT(LOG_INFO, "Start channel " << ch << " for " << t << " minutes" );

            GpioRelay *R = new GpioRelay(ch);

            R->Start();
            sleep(t*60);
            R->Stop();

            delete R;

            exit(0);
        }
    }
}

int main(int argc, char* argv[])
{

    ParseOptions(argc, argv);

    // Register signals
    signal(SIGINT,  sighandler_stop);
    signal(SIGKILL, sighandler_stop);
    signal(SIGTERM, sighandler_stop);


    // Load configuration and create 
    {
        ifstream cf("config.json");
        string config_str((std::istreambuf_iterator<char>(cf)),
                         std::istreambuf_iterator<char>());
        cf.close();

        StaticJsonBuffer<512> jbuf;
        JsonObject& root = jbuf.parseObject(config_str);

        if(!root.success())
        {
            DEBUG_PRINT(LOG_WARNING, "Error parsing config.json");
            exit(-1);
        }
        
        int n_s = root["schedule"].size();
        DEBUG_PRINT(LOG_INFO, "Loading schedule configuration (ch# " << n_s << ")" );
        for(int i=0; i<n_s; ++i)
        {
            if(root["schedule"][i]["type"] == string("ical"))
            {
                DEBUG_PRINT(LOG_INFO, "  - ical, gpio " << root["schedule"][i]["igpo"][0]);
                // Adding ical type schedule
                //cout << root["schedule"][i]["url"] << endl;
                string gpio_id = root["schedule"][i]["igpo"][0];
                shared_ptr<channel_t> vt(new channel_t(root["schedule"][i]["url"],std::stoi(gpio_id)) );

                //cout << "$ filename " << vt->get_cache_filename() << endl;
                if(!vt->load_cached(UPDATE_TIME)) {
                    //cout << "Load from web\n";
                    vt->load_from_url();
                }
                valves.push_back(vt);
            }
        }
    }

    time_t last_reload = time(0); //-2*UPDATE_TIME; // Hack to force the reload on the first iteration

    // Enter into work loop
    while(true)
    {
        bool to_reload;
        if( to_reload = (time(0) - last_reload) > UPDATE_TIME )
            last_reload = time(0);
        for( auto &vt : valves)
        {
            if(to_reload)
            {
                int err;
                if(err=vt->load_from_url() !=NET_SUCCESS) {
                    DEBUG_PRINT(LOG_INFO, "ERROR: Error loading ical" );
                }
            }

            DEBUG_PRINT(LOG_INFO, "INFO: update status");
            if(vt->vc.IsActive() != vt->gpio.GetStatus())
                vt->gpio.SetStatus(vt->vc.IsActive());
        }

        sleep(CHECK_TIME);
    }
    
    return 0;
}

