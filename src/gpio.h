#ifndef GPIO_H
#define GPIO_H

class GpioRelay
{
public:
	GpioRelay(int GpioPIN); //: is_on(false), pin_id(GpioPIN);
	~GpioRelay();

	bool GetStatus() { return is_on; };

	int  SetStatus(bool on);
	int  GetPin() { return pin_id; }

	void Start() { SetStatus(true);  }
	void Stop()  { SetStatus(false); }

private:
	bool is_on;

	int   pin_id;
	void *handler;
};

#endif

