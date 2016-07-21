#include "valve.h"
#include "net.h"

#include <stdio.h>
#include <string>
#include <cyassl/ctaocrypt/md5.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

string get_cache_filename();

string valve_t::get_hash()
{

	byte md5sum[MD5_DIGEST_SIZE];
	char r[MD5_DIGEST_SIZE*2+1];

	Md5 md5;
	InitMd5(&md5);

	Md5Update(&md5, (const byte *)this->url.c_str(), this->url.length());
	Md5Final(&md5, md5sum);

	r[MD5_DIGEST_SIZE*2] = 0;
	for(int i=0;i<MD5_DIGEST_SIZE;++i)
		sprintf(&r[2*i],"%02X",md5sum[i]);
	return r;
}

std::string valve_t::get_cache_filename()
{
	return "./"+this->get_hash()+".ics";
}

bool valve_t::load_cached(int max_age)
{
	// check if $ file exists
	struct stat st;
	std::string f_name = this->get_cache_filename();
	if(stat(f_name.c_str(),&st) == 0)
	{
		if( time(0) - st.st_mtime > max_age )
			return false;
		this->vc.ParseICALFromFile(f_name);
		return true;
	}
	else
		return false;
}

int valve_t::load_from_url(bool store_to_cache)
{
	std::string ical_string="";
	int err;
	if( (err = load_ical_from_url(ical_string, this->url, 0)) == NET_SUCCESS)
	{
	    this->vc.ParseICALFromString(ical_string);
	    if(store_to_cache) {
	    	ofstream f(this->get_cache_filename());
	    	f << ical_string;
	    	f.close();
	    }
	}
	return err;
}

int set_valve_status(unsigned int id, bool active)
{
	printf("INFO: Set valve #%d status to %d\n", id, active);
	return 0;
}
