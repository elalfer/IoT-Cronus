#include "cal.h"

//// TODO list
// * Preserve current list of events or replace - current solution is to replace. Might lead to broken list...
// * Adjust time for iCal control based on the "grass type"
// 

#include <algorithm>
#include <sys/stat.h>

void iCalValveControl::add_to_list(icaltimetype start, icaltimetype end)
{
	this->events.push_back(icaltime_span_new(start,end,1));
}

void iCalValveControl::sort_events()
{

}

inline bool in_range_pred(const icaltime_span &t_span)
{
	// FIXME - make sure it is inlined and doesn't get called every time
	time_t ct = time(0);
	return (t_span.start >= ct) && (t_span.end <= ct);
}

bool iCalValveControl::IsActive() {
	// FIXME - We can do binary_rearch if all elements are sorted (by start time)
	//	What if some of the time spans do overlap
	std::vector<icaltime_span>::iterator v = find_if(std::begin(events), std::end(events), in_range_pred );
	return v != events.end();
}

// Parse calendar information from the string
int iCalValveControl::ParseICALFromString(const string &ical)
{
	printf("INFO: Parsing schedule\n");

	VComponent ic(icalparser_parse_string((const char *)ical.c_str()));

    if(ic.is_valid())
    {
    	// Remove all event if ical is valid
    	this->events.clear();

        //cout << "Parsing component: " << ic.is_valid() << endl;
        //cout << ic.as_ical_string().c_str() << endl;

        // Go through all events and pring time
        VEvent *sub_ic = dynamic_cast<VEvent *>(ic.get_first_component(ICAL_VEVENT_COMPONENT));

        //cout << "=========== Getting VEvent subcomponent ==========\n" << 
        //       sub_ic->isa() << endl <<
        //       ICAL_VEVENT_COMPONENT << endl;

        while (sub_ic != NULL) {
            //printf("subcomponent: %s\n", sub_ic->as_ical_string().c_str());

            //cout << "From: " << icaltime_as_ical_string(sub_ic->get_dtstart())      << endl;
            //cout << "To:   " << icaltime_as_ical_string(sub_ic->get_dtend())        << endl;

            icaltimetype dt_today = icaltime_today(); //icaltime_current_time_with_zone();
            icaltimetype dt_max_today = icaltime_today();
            	icaltime_adjust(&dt_max_today, MAX_DAYS_IN_FUTURE, 0, 0, 0); // Current date + MAX_DAYS_IN_FUTURE
            icaltimetype dt_now = icaltime_from_timet(time(0), false);

            //cout << "Today:" << icaltime_as_ical_string(dt_today) << endl;
            //cout << "Now:  " << icaltime_as_ical_string(dt_now) << endl;

            ICalProperty *rec_rule = sub_ic->get_first_property(ICAL_RRULE_PROPERTY);

            // If rec rule
            if(rec_rule)
            {
                icaltimetype rec_dt_start = sub_ic->get_dtstart();
                icaltimetype rec_dt_end   = sub_ic->get_dtend();

                //cout << "Rec rule exists: " << rec_rule->as_ical_string();
                //cout << "  start " << icaltime_as_ical_string(rec_dt_start) << endl;
                // Construct recurent iterator

                struct icalrecurrencetype recur = rec_rule->get_rrule();
                //cout << "  rec " << icalrecurrencetype_as_string(&recur) << endl;

                icalrecur_iterator *ritr, *ritr_end;
                ritr = icalrecur_iterator_new(recur, rec_dt_start);
                ritr_end = icalrecur_iterator_new(recur, rec_dt_end);

                struct icaltimetype next = icalrecur_iterator_next(ritr);
                struct icaltimetype next_e = icalrecur_iterator_next(ritr_end);

                // Check until today's date is found
                while (!icaltime_is_null_time(next) && icaltime_compare_date_only(next, dt_today) < 0 ){
                        next   = icalrecur_iterator_next(ritr);
                        next_e = icalrecur_iterator_next(ritr_end);
                }

                // Get next few accurancies and add to the list
                do {
                		//cout << "  next " << icaltime_as_ical_string(next) << " - " << icaltime_as_ical_string(next_e) << endl;
                        next = icalrecur_iterator_next(ritr);
                        next_e = icalrecur_iterator_next(ritr_end);

                        add_to_list(next, next_e);
                }
                while (!icaltime_is_null_time(next) && icaltime_compare_date_only(next, dt_max_today) < 0 );

                icalrecur_iterator_free(ritr);
            }
            else
            {
                // Just check the time
                if(icaltime_compare_date_only(sub_ic->get_dtstart(),dt_max_today) == 0)
                {
                    //cout << "  next " << icaltime_as_ical_string(next) << endl;
                }

                // TODO: Do we need to sort all future events? can they come unordered?
            }

            sub_ic = dynamic_cast<VEvent *>(ic.get_next_component(ICAL_VEVENT_COMPONENT));
        }

        // Sort the events
        this->sort_events();
        this->last_updated = time(0);

        return 0;
    }
    else
    {
        printf("ERROR: Parsing error!!!!\n");
        return -1;
    }
}

// Load and parse calendar information from file
int iCalValveControl::ParseICALFromFile(string file_name)
{
    ifstream f(file_name);
    string ln;
    string ical("");
    bool v_cal_output = false;
    while(std::getline(f,ln))
    {
        if (ln.compare(0,13,"END:VCALENDAR") == 0)
        {
            ical += "END:VCALENDAR";
            break;
        }
        if (ln.compare(0,15,"BEGIN:VCALENDAR") == 0)
            v_cal_output = true;
        
        if(v_cal_output)
            ical += ln + "\n";
    }
    f.close();
    int ret = this->ParseICALFromString(ical);

    // Set last_update time to file modification time
    struct stat cal_file_stat;
    stat(file_name.c_str(), &cal_file_stat);
    this->last_updated = cal_file_stat.st_mtimespec.tv_sec;

    return ret;
}

