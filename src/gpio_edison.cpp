#include "gpio.h"

#include <stdlib.h>
#include <mraa/gpio.hpp>

GpioRelay::GpioRelay(int GpioPIN)
{

	pin_id=GpioPIN;
	is_on=false;

	mraa::Gpio* gpio = new mraa::Gpio(GpioPIN);
	if (!gpio) {
		fprintf(stderr, "ERROR [Fatal]: can not connect to gpio port %d\n", GpioPIN);
		exit(-1);
	}
	mraa::Result response = gpio->dir(mraa::DIR_OUT);
	if (response != mraa::SUCCESS) {
		// mraa::printError(response);
		fprintf(stderr, "ERROR [Fatal]: can not connect to gpio port %d\n", GpioPIN);
		exit(-1);
	}

	this->handler = gpio;
}

GpioRelay::~GpioRelay()
{
	this->SetStatus(false);
	DEBUG_PRINT(LOG_INFO, "Delete Gpio device #" << this->pin_id);

	delete static_cast<mraa::Gpio*>(this->handler);
}

int GpioRelay::SetStatus(bool on)
{
	DEBUG_PRINT(LOG_INFO, "Set pin #" << this->pin_id << " state to " << on );

	static_cast<mraa::Gpio*>(this->handler)->write(on?1:0);
	is_on = on;
}

