#ifndef __galileo_tracker_api_h__
#define __galileo_tracker_api_h__

#include "orbit_propagator.h"

#ifndef __TRACKER_API_DEFINES
#define __TRACKER_API_DEFINES

#define DEBUG_LEVEL 1

#ifdef DEBUG
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif
#define _api_printf(level, x...) do{ if(level >= DEBUG_LEVEL) printf(x); }while(0);
#else
#define _api_printf(level, x...) do{ }while(0);
#endif

#define NORMAL_BUFFER_SIZE          256
#define LONG_BUFFER_SIZE            1024
#define SHORT_BUFFER_SIZE           64

#define L1_FREQ                     1575.42e6
#define L2_FREQ                     1227.60e6

typedef struct object_motion_s{
    vec3 pos;
    vec3 vel;
}object_motion_t;

#endif

#define GALILEO_CONSTELLATION_AMOUNT    50


typedef struct galileo_constellation_s{
    /* More shit can be added here */
    int     found[GALILEO_CONSTELLATION_AMOUNT];
    double  doppler[GALILEO_CONSTELLATION_AMOUNT];
}galileo_constellation_t;

typedef struct galileo_search_parameters_s{
    char    tle_file[NORMAL_BUFFER_SIZE];
    char    norad_to_prn_file[NORMAL_BUFFER_SIZE];
    double  minimum_angle;
    int     max_satellite_amount;
    double  frequency;
}galileo_search_parameters_t;

int get_visible_galileo_from_tle_file(  galileo_search_parameters_t * gal_search, 
                                        object_motion_t * receiver, galileo_constellation_t * res);

#endif
