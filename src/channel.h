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

#ifndef VALVE_H
#define VALVE_H

#include <string>

#include "cal.h"
#include "gpio.h"

using namespace std;

// Type declaration
struct channel_t
{
    iCalValveControl vc;
    GpioRelay gpio;
    const string url;

    channel_t(string ical_url, int gpio_pin) : gpio(gpio_pin), vc(GT_DEFUALT), url(ical_url) {}

    string get_hash();
    string get_cache_filename();
    bool load_cached(int max_age = 360);
    int load_from_url(bool store_to_cache = true);


};

#endif