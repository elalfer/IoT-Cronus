#include "gpio.h"

GpioRelay::GpioRelay(int GpioPIN)
{
	this->pin_id = GpioPIN;
}

GpioRelay::~GpioRelay()
{
}

int GpioRelay::SetStatus(bool on)
{
	is_on = on;
	return 1;
}

