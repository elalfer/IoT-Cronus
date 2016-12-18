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

#include "gpio.h"
#include "log.h"

GpioRelay::GpioRelay(int GpioPIN)
{
	this->pin_id = GpioPIN;
	this->is_on = 0;
}

const bool GpioRelay::GetHWStatus() {
	return this->is_on;
}

GpioRelay::~GpioRelay()
{
	this->SetStatus(false);
	DEBUG_PRINT(LOG_INFO, "Delete Gpio device #" << this->pin_id);
}

int GpioRelay::SetStatus(bool on)
{
	is_on = on;

	DEBUG_PRINT(LOG_INFO, "Change pin #" << this->pin_id << " state to " << on );

	return 1;
}

