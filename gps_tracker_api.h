#ifndef __gps_tracker_api_h__
#define __gps_tracker_api_h__

#include "orbit_propagator.h"

#define DEBUG_LEVEL 1

#ifdef DEBUG
#ifndef	DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif
#define _api_printf(level, x...) do{ if(level >= DEBUG_LEVEL) printf(x); }while(0);
#else
#define _api_printf(level, x...) do{ }while(0);
#endif

#define GPS_CONSTELLATION_AMOUNT 	32

#define NORMAL_BUFFER_SIZE 			256
#define LONG_BUFFER_SIZE 			1024
#define SHORT_BUFFER_SIZE			64

#define L1_FREQ						1575.42e6
#define L2_FREQ						1227.60e6

typedef struct object_motion_s{
	vec3 pos;
	vec3 vel;
}object_motion_t;

typedef struct gps_constellation_s{
	/* More shit can be added here */
	int 	found[GPS_CONSTELLATION_AMOUNT];
	double 	doppler[GPS_CONSTELLATION_AMOUNT];
}gps_constellation_t;

typedef struct gps_search_parameters_s{
	char 	tle_file[NORMAL_BUFFER_SIZE];
	char 	norad_to_prn_file[NORMAL_BUFFER_SIZE];
	double 	minimum_angle;
	int 	max_satellite_amount;
	double 	frequency;
}gps_search_parameters_t;

int get_receiver_motion(const char * tle_file, vec3 * llh_pos, object_motion_t * mot);
int get_visible_gps_from_tle_file(gps_search_parameters_t * gps_search, object_motion_t * receiver, gps_constellation_t * res);

#endif
