#ifndef GPIO_H
#define GPIO_H

class GpioRelay
{
public:
	GpioRelay(int GpioPIN); //: is_on(false), pin_id(GpioPIN);
	~GpioRelay();

	bool IsOn() { return is_on; };

	int  SetStatus(bool on);
	int  GetPin() { return pin_id; }

private:
	bool is_on;

	int   pin_id;
	void *handler;
};

#endif

