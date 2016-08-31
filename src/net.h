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

#ifndef NET_H
#define NET_H

#include <string>
#include <time.h>

// Network error codes
#define NET_SUCCESS 		0
#define NET_NOT_CHANGED 	1
#define NET_ERROR 			-1

// Load ical from URL
int load_ical_from_url(std::string &ical, const std::string &URL, time_t mod_since);

std::string hostname_from_url(const std::string &url);

#endif