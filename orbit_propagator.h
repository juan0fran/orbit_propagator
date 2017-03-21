#ifndef __ORBIT_PROPAGATOR_H__
#define __ORBIT_PROPAGATOR_H__

#include "sgdp4.h"
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <math.h>


/* Geodetic transf. defines */
#define  OMEGAE  7.29211586e-5              //  Earth rotation rate in rad/s
#define F 1/298.257223563                   //  WGS-84 Flattening.
#define ECCN (F*(2.0 - F))              //  Eccentricity.
#define R_0 (double)6378137                 //  WGS-84 equatorial radius (m).
#define R_P (double)R_0*(1 - Fwgs_84)       //  Polar radius (m).

/* Defines for ecef2lla */
#define A 6378137.0
#define B 6356752.3142

#define YAW_TYPE 1
#define PITCH_TYPE 2
#define ROLL_TYPE 3

#define PI M_PI

#define PI180   (PI/180.0)
#define M2KM 1/1000
#define KM2M 1000

#define sind(x) (sin(fmod((x),360) * PI / 180))
#define asind(x) (asin( x ) / PI * 180)
#define cosd(x) (cos(fmod((x),360) * PI / 180))
#define acosd(x) (acos( x ) / PI * 180)

#define     DEG2RAD(x)      (x * M_PI) / 180.0   

typedef union __attribute__ ((__packed__)) _vec3{
	double raw[3];
	struct __attribute__ ((__packed__)) {
		double x;
		double y;
		double z;
	}f;
}vec3;

typedef union __attribute__ ((__packed__)) _mat3{
	double raw[3][3];
	struct __attribute__ ((__packed__)) {
		double _11;
		double _12;
		double _13;
		double _21;
		double _22;
		double _23;
		double _31;
		double _32;
		double _33;		
	}f;
}mat3;

#define TLE_LINE_SIZE 	69
#define TLE_NAME_SIZE 	24
#define TLE_EOLINE_SIZE	1

typedef char 	tle_line[TLE_LINE_SIZE + TLE_EOLINE_SIZE];
typedef char 	tle_name[TLE_NAME_SIZE + TLE_EOLINE_SIZE];

typedef union __attribute__ ((__packed__)) _tle_set{
	char raw[TLE_NAME_SIZE + TLE_LINE_SIZE + TLE_LINE_SIZE + 3*TLE_EOLINE_SIZE];
	struct __attribute__ ((__packed__)) {
		tle_name _name;
		tle_line _1;
		tle_line _2;
	}lines;
}tle_set;

extern double SGDP4_jd0;

int 		actualposition(double jd_actual, tle_set * tle, vec3 *sat_eci, vec3 *sat_vel);
double 		j_day(unsigned int unix_timestamp);
double 		angle_vec3(vec3 a, vec3 b);

void 		eci2llh(double jd_actual, vec3 eci, vec3 *llh);
void 		ecef2llh(vec3 ecef, vec3 *llh);
void 		llh2ecef(vec3 llh, vec3 *ecef);
void 		llh2eci(double jd_actual, vec3 llh, vec3 *eci);

#endif
