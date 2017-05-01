#ifndef __PAYLOAD_TRACKER_API_H__
#define __PAYLOAD_TRACKER_API_H__

#include "orbit_propagator.h"
#include <stdbool.h>
#include <time.h>

#define DEBUG_LEVEL 1

#ifdef DEBUG
#ifndef	DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif
#define _api_printf(level, x...) do{ if(level >= DEBUG_LEVEL) printf(x); }while(0);
#else
#define _api_printf(level, x...) do{ }while(0);
#endif

#define NORMAL_BUFFER_SIZE 			256
#define LONG_BUFFER_SIZE 			1024
#define SHORT_BUFFER_SIZE			64

typedef struct square_region_s{
    vec3 p1;
    vec3 p2;
    vec3 p3;
    vec3 p4;
}square_region_t;

typedef struct object_motion_s{
	vec3 pos;
	vec3 vel;
}object_motion_t;

typedef struct payload_location_s{
	double az;
	double el;
	double dist;
}payload_location_t;

typedef struct propagation_config_s{
    time_t      timestamp;
    /* If false, look for station_llh */
    /* If true, look for tle set */
    bool 		has_tle;
    vec3 		llh;
    char 		tle[NORMAL_BUFFER_SIZE];
}propagation_config_t;

typedef struct visibility_config_t{
    propagation_config_t station;
    propagation_config_t platform;
	/* Frequency of tx/rx, */
	double 		in_freq;
}visibility_config_t;

typedef struct propagation_output_s{
	/* In case you have a tle... */
	vec3 		platform_llh;
	vec3 		station_llh;
	/*output the frequency that you have to TX/RX to compensate dopplers */
	double 		doppler;
	/* Relative speed and distance */
	double 		rel_velocity;
	double 		rel_dist;
	/* Az and El from GS in degrees */
	double 		az;
	double 		el;
}propagation_output_t;

int propagate_and_get_visibility(visibility_config_t * conf, propagation_output_t * res);
int propagate_and_get_llh(propagation_config_t *conf, vec3 * llh);
bool ispoint_inside_region(vec3 pt, square_region_t reg);

#endif
