#include "cal.h"
#include "log.h"
#include <time.h>

//// TODO list
// * Preserve current list of events or replace - current solution is to replace. Might lead to broken list...
// * Adjust time for iCal control based on the "grass type"
// * Do we need to sort all future events? can they come unordered?
// * Implement sort_events if needed

#include <algorithm>
#include <sys/stat.h>

void iCalValveControl::add_to_list(icaltimetype start, icaltimetype end)
{
    struct icaltime_span ts = icaltime_span_new(start,end,1);
	this->events.push_back(ts);
}

void iCalValveControl::sort_events()
{

}

bool iCalValveControl::IsActive() {
    time_t now = time(0);
    auto v = find_if(std::begin(events), std::end(events),
       [now](const icaltime_span &ts) {
            return (ts.start <= now) && (now <= ts.end);
       } );
	return v != events.end();
}

// Parse calendar information from the string
int iCalValveControl::ParseICALFromString(const string &ical)
{
	DEBUG_PRINT(LOG_DEBUG, "INFO: Parsing schedule\n");

	VComponent ic(icalparser_parse_string((const char *)ical.c_str()));

    if(ic.is_valid())
    {
    	// Remove all event if ical is valid
    	this->events.clear();

        DEBUG_PRINT(LOG_DEBUG, "Parsing iCal component: " << ic.is_valid());
        //DEBUG_PRINT(LOG_INFO, ic.as_ical_string().c_str());

        // Go through all events and pring time
        VEvent *sub_ic = dynamic_cast<VEvent *>(ic.get_first_component(ICAL_VEVENT_COMPONENT));

        //cout << "=========== Getting VEvent subcomponent ==========\n" << 
        //       sub_ic->isa() << endl <<
        //       ICAL_VEVENT_COMPONENT << endl;

        while (sub_ic != NULL) {
            DEBUG_PRINT(LOG_DEBUG, "  Subcomponent");

            DEBUG_PRINT(LOG_DEBUG, "    From: " << icaltime_as_ical_string(sub_ic->get_dtstart()) );
            DEBUG_PRINT(LOG_DEBUG, "    To:   " << icaltime_as_ical_string(sub_ic->get_dtend()) );

            icaltimetype dt_today = icaltime_today(); //icaltime_current_time_with_zone();
            icaltimetype dt_max_today = icaltime_today();
            	icaltime_adjust(&dt_max_today, MAX_DAYS_IN_FUTURE, 0, 0, 0); // Current date + MAX_DAYS_IN_FUTURE
            // icaltimetype dt_now = icaltime_from_timet(time(0), false);

            //cout << "Today:" << icaltime_as_ical_string(dt_today) << endl;
            //cout << "Now:  " << icaltime_as_ical_string(dt_now) << endl;

            ICalProperty *rec_rule = sub_ic->get_first_property(ICAL_RRULE_PROPERTY);

            // If rec rule
            if(rec_rule)
            {
                icaltimetype rec_dt_start = sub_ic->get_dtstart();
                icaltimetype rec_dt_end   = sub_ic->get_dtend();

                DEBUG_PRINT(LOG_DEBUG, "    REC rule: " << rec_rule->as_ical_string() );
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
                		DEBUG_PRINT(LOG_DEBUG, "    next " << icaltime_as_ical_string(next) << " - " << icaltime_as_ical_string(next_e) );
                        next = icalrecur_iterator_next(ritr);
                        next_e = icalrecur_iterator_next(ritr_end);

                        // Add new event to the list
                        add_to_list(next, next_e);
                }
                while (!icaltime_is_null_time(next) && icaltime_compare_date_only(next, dt_max_today) < 0 );

                icalrecur_iterator_free(ritr);
            }
            else
            {
            	DEBUG_PRINT(LOG_DEBUG, "    Single time event");

                //cout << "Time range: " << icaltime_as_ical_string(dt_today) << " - " << icaltime_as_ical_string(dt_max_today) << endl;

                // Just check the time
                if( (icaltime_compare_date_only(sub_ic->get_dtstart(),dt_max_today) < 0) &&
                	(icaltime_compare_date_only(sub_ic->get_dtend(),dt_today) >= 0) )
                {
                	// Add single event to the list
                	add_to_list(sub_ic->get_dtstart(), sub_ic->get_dtend());
                }

            }

            sub_ic = dynamic_cast<VEvent *>(ic.get_next_component(ICAL_VEVENT_COMPONENT));
        }

        // Sort the events
        this->sort_events();
        this->last_updated = time(0);

        DEBUG_PRINT(LOG_DEBUG, "Added events:");
        for(auto ts: this->events)
            DEBUG_PRINT(LOG_DEBUG, "      - " << format_timespan_t(ts.start,ts.end) );

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
    //this->last_updated = cal_file_stat.st_mtimespec.tv_sec;

    return ret;
}

