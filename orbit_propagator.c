#include "orbit_propagator.h"

int sign(double a){
    if (a < 0.0)
        return -1;
    if (a > 0.0)
        return 1;
    return 0;
}
/* 3 element vector operations */
void zero_vec3(vec3 *res){
    res->raw[0] = 0;
    res->raw[1] = 0;
    res->raw[2] = 0;
}

void copy_vec3(vec3 src, vec3 *dst) {
    /* Avoid small loops to be slightly more efficient */
    dst->raw[0] = src.raw[0];
    dst->raw[1] = src.raw[1];
    dst->raw[2] = src.raw[2];
}

void add_vec3(vec3 a, vec3 b, vec3 *res) {
    /* Avoid small loops to be slightly more efficient */
    res->raw[0] = a.raw[0] + b.raw[0];
    res->raw[1] = a.raw[1] + b.raw[1];
    res->raw[2] = a.raw[2] + b.raw[2];
}

void sub_vec3(vec3 a, vec3 b, vec3 *res) {
    /* Avoid small loops to be slightly more efficient */
    res->raw[0] = a.raw[0] - b.raw[0];
    res->raw[1] = a.raw[1] - b.raw[1];
    res->raw[2] = a.raw[2] - b.raw[2];
}

void mult_vec3_scalar(vec3 a, double b, vec3 *res) {
    /* Avoid small loops to be slightly more efficient */
    res->raw[0] = a.raw[0] * b;
    res->raw[1] = a.raw[1] * b;
    res->raw[2] = a.raw[2] * b;
}

void prod_vec3_pos_by_pos(vec3 a, vec3 b, vec3 *res)
{
    /* Avoid small loops to be slightly more efficient */
    res->raw[0] = a.raw[0] * b.raw[0];
    res->raw[1] = a.raw[1] * b.raw[1];
    res->raw[2] = a.raw[2] * b.raw[2];
}

void prod_vec3_raw_vec3_col(vec3 a, vec3 b, double *res)
{
    (*res) = a.raw[0] * b.raw[0] + a.raw[1] * b.raw[1] + a.raw[2] * b.raw[2];
}


void norm_vec3(vec3 v, double *res)
{
    /* Vector length */
    *res = sqrt((v.raw[0] * v.raw[0]) + (v.raw[1] * v.raw[1]) + (v.raw[2] * v.raw[2]));
}

void vec3_normalized(vec3 vec, vec3 *res)
{
    double norm;
    norm_vec3(vec, &norm);
    
    res->raw[0] = vec.raw[0] / norm;
    res->raw[1] = vec.raw[1] / norm;
    res->raw[2] = vec.raw[2] / norm;
}

void max_vec3(vec3 a, vec3 b, vec3 *res)
{
    res->raw[0] = fmax(a.raw[0], b.raw[0]);
    res->raw[1] = fmax(a.raw[1], b.raw[1]);
    res->raw[2] = fmax(a.raw[2], b.raw[2]);
}

void zero_mat3(mat3 *a) {
    /* Avoid small loops to be slightly more efficient */
    a->raw[0][0] = 0;
    a->raw[0][1] = 0;
    a->raw[0][2] = 0;
    a->raw[1][0] = 0;
    a->raw[1][1] = 0;
    a->raw[1][2] = 0;
    a->raw[2][0] = 0;
    a->raw[2][1] = 0;
    a->raw[2][2] = 0;
}

void mult_mat3_vec3(mat3 a, vec3 b, vec3 *res)
{
    /* Unsafe if b = res->raw */
    /* Avoid small loops to be slightly more efficient */
    res->raw[0] = (a.raw[0][0] * b.raw[0]) + (a.raw[0][1] * b.raw[1]) + (a.raw[0][2] * b.raw[2]);
    res->raw[1] = (a.raw[1][0] * b.raw[0]) + (a.raw[1][1] * b.raw[1]) + (a.raw[1][2] * b.raw[2]);
    res->raw[2] = (a.raw[2][0] * b.raw[0]) + (a.raw[2][1] * b.raw[1]) + (a.raw[2][2] * b.raw[2]);
}

void mult_vec3_mat3(vec3 a, mat3 b, vec3 *res)
{
    res->raw[0] = (a.raw[0] * b.raw[0][0]) + (a.raw[1] * b.raw[1][0]) + (a.raw[2] * b.raw[2][0]);
    res->raw[1] = (a.raw[0] * b.raw[0][1]) + (a.raw[1] * b.raw[1][1]) + (a.raw[2] * b.raw[2][1]);
    res->raw[2] = (a.raw[0] * b.raw[0][2]) + (a.raw[1] * b.raw[1][2]) + (a.raw[2] * b.raw[2][2]);
}

void mult_mat3_scalar(mat3 a, double b, mat3 *res)
{
    /* Avoid small loops to be slightly more efficient */
    res->raw[0][0] = a.raw[0][0] * b;
    res->raw[0][1] = a.raw[0][1] * b;
    res->raw[0][2] = a.raw[0][2] * b;
    res->raw[1][0] = a.raw[1][0] * b;
    res->raw[1][1] = a.raw[1][1] * b;
    res->raw[1][2] = a.raw[1][2] * b;
    res->raw[2][0] = a.raw[2][0] * b;
    res->raw[2][1] = a.raw[2][1] * b;
    res->raw[2][2] = a.raw[2][2] * b;
}

/* A is the GPS Point */
/* B is the Receiver Point */
/* All is in ECI coordinates */
/* To get the vector from Rx to GPS: DST = GPS-RX */
/* After that compute angle between both */
double angle_vec3(vec3 a, vec3 b)
{
    double res, norm_a, norm_b, arc;
    vec3 dst;
    sub_vec3(a, b, &dst);
    prod_vec3_raw_vec3_col(dst, b, &res);
    norm_vec3(dst, &norm_a);
    norm_vec3(b, &norm_b);
    arc = res/(norm_a*norm_b);
    if (arc > 1.0 || arc < -1.0){
        if (arc > 1.0){
            arc = 1.0 - 1e-10;
        }else{
            arc = -1.0 + 1e-10;
        }
    }
    return (acosd(arc));
}

static void greenwidtchtime(double jd, double *gst);

void ecef2eci(double gst, vec3 vec_ecef, vec3 *vec_eci)
{
    vec_eci->raw[0] = cos(gst) * vec_ecef.raw[0] - sin(gst) * vec_ecef.raw[1];
    vec_eci->raw[1] = sin(gst) * vec_ecef.raw[0] + cos(gst) * vec_ecef.raw[1];
    vec_eci->raw[2] = vec_ecef.raw[2];
}

void eci2ecef(double gst, vec3 vec_eci, vec3 *vec_ecef)
{
    double CGAST, SGAST, x_eci, y_eci, z_eci;
    
    x_eci = vec_eci.raw[0];
    y_eci = vec_eci.raw[1];
    z_eci = vec_eci.raw[2];
    
    //Now rotate the coordinates
    CGAST = cos(gst);// * PI180);
    SGAST = sin(gst);// * PI180);
    
    vec_ecef->raw[0] =  x_eci * CGAST + (y_eci * SGAST);
    vec_ecef->raw[1] = -x_eci * SGAST + (y_eci * CGAST);
    vec_ecef->raw[2] =  z_eci;
}

void get_dircos(int matflavor, double angle, mat3 *dcm)
{
    /*
     Fills a direction cosine matrix defined by positive right-hand rule Euler angles that transforms from an INS type basis to a body type basis.
     
     Inputs:
     A            - Angle in Radians
     MatrixFlavor - Axis: ROLL_TYPE, PITCH_TYPE, YAW_TYPE
     
     Outputs:
     DC           - Direction Cosine Matrix
     */
    
    zero_mat3(dcm);
    
    switch (matflavor)
    {
        case YAW_TYPE:
            dcm->raw[0][0] = cos(angle);
            dcm->raw[0][1] = sin(angle);
            dcm->raw[1][0] = -sin(angle);
            dcm->raw[1][1] = cos(angle);
            dcm->raw[2][2] = 1;
            break;
            
        case PITCH_TYPE:
            dcm->raw[0][0] = cos(angle);
            dcm->raw[0][2] = -sin(angle);
            dcm->raw[1][1] = 1;
            dcm->raw[2][0] = sin(angle);
            dcm->raw[2][2] = cos(angle);
            break;
            
        case ROLL_TYPE:
            dcm->raw[0][0] = 1;
            dcm->raw[1][1] = cos(angle);
            dcm->raw[1][2] = sin(angle);
            dcm->raw[2][1] = -sin(angle);
            dcm->raw[2][2] = cos(angle);
            break;
    }
}

void ecef2uvw(vec3 ecef, vec3 origin, vec3 *uvw)
{
    /*
     This function will rotate ECF coordinates into UVW coordinates: X axis (U axis) is colinear with longitude of origin
     */
    
    mat3 dcm;
    
    get_dircos(YAW_TYPE, origin.raw[1], &dcm);
    mult_mat3_vec3(dcm, ecef, uvw);
}

void uvw2llh(vec3 uvw, vec3 origin, vec3 *llh)
{
    /*
     This function will convert a UVW coordinates vector to geodetic LLH coordinates (Longitude, Latitude, Height)
     */
    
    vec3 tmp_llh;
    double denom, N, esqNsin, dNdlat, tmp, tmp2, re, tmp_lat, dlat, olatsav;
    
    copy_vec3(origin, &tmp_llh);
    
    /*
     Radius of curvature of ellipsoid in a plane perpendicular to a meridian and perpendicular to a plane tangent to the surface. The value N here is for the origin and is used as the initial value in the iterations in the geodetic to uvw transformation.
     */
    
    denom = 1.0 - ECCN * sin(tmp_llh.raw[0]) * sin(tmp_llh.raw[0]);
    N = A / sqrt( denom );
    
    /*
     Compute the offset of the geodetic and geocentric centers - a magic number first guess.
     */
    
    esqNsin = ECCN * N * sin(tmp_llh.raw[0]);
    
    /* Compute derivative of N with latitude as help for first guess */
    
    dNdlat = esqNsin * cos(tmp_llh.raw[0]) / denom;
    tmp = sqrt(uvw.raw[0] * uvw.raw[0] + uvw.raw[1] * uvw.raw[1]);
    
    if(tmp == 0) /* At North or South Pole */
    {
        if(uvw.raw[2] > 0.0)
        {
            tmp_llh.raw[2] = uvw.raw[2] - (A / sqrt(1 - ECCN));
            tmp_llh.raw[0] = asin(1);
            
        }else{
            tmp_llh.raw[2] = -uvw.raw[2] - (A / sqrt(1 - ECCN));
            tmp_llh.raw[0] = asin(-1);
        }
    }else{ /* Position is NOT at the Pole */
        tmp_llh.raw[1] = tmp_llh.raw[1] + atan2(uvw.raw[1], uvw.raw[0]);
        
        /* Take initial guess at latitude and effective radius, then iterate */
        tmp_lat = atan2( uvw.raw[2] + esqNsin, tmp);
        
        /* radius of earth in meters */
        re = N + dNdlat * (tmp_lat - tmp_llh.raw[0]);
        
        dlat = 1.0;
        /* Go until roughly half meter error on surface i.e. 1e-7 * 6.38e6 */
        
        while ( dlat > 1.0e-7 )
        {
            olatsav = tmp_lat;
            tmp2 = uvw.raw[2] + ECCN * re * sin(tmp_lat);
            tmp_lat = atan2(tmp2, tmp);
            re = A / sqrt(1.0 - ECCN * sin(tmp_lat) * sin(tmp_lat));
            dlat = fabs(tmp_lat - olatsav);
        }
        
        tmp_llh.raw[2] = tmp / cos(tmp_lat) - re; /* height in meters */
        tmp_llh.raw[0] = tmp_lat;
    }
    
    copy_vec3(tmp_llh, llh);
}

void ecef2llh(vec3 ecef, vec3 *llh)
{
    /*
     This function returns the [lat lon hgt] LLH coordinates of the given earth center fixed (ECF) coordinates.
     
     - latitude (positive): -90/90 [deg]
     - longitude (positive East): -180/180 [deg]
     - height [m]
     */
    vec3 origin, uvw, tmp_llh;
    zero_vec3(&origin);
    
    ecef2uvw(ecef, origin, &uvw);
    
    uvw2llh(uvw, origin, &tmp_llh);
    
    llh->raw[0] = tmp_llh.raw[0] * (180.0/PI); /* latitude in degrees -90 - 90*/
    llh->raw[1] = tmp_llh.raw[1] * (180.0/PI); /* longitude in degrees -180 - 180 */
    llh->raw[2] = tmp_llh.raw[2];              /* height in meters */
}

void eci2llh(double jd_actual, vec3 eci, vec3 *llh)
{
    vec3 ecef;
    double gst;
    /* compute GST value */
    greenwidtchtime(jd_actual, &gst);
    eci2ecef(gst, eci, &ecef);
    ecef2llh(ecef, llh);
}

void llh2ecef(vec3 llh, vec3 * ecef)
{
    #define a_lla2ecef  6378137.0
    #define b_lla2ecef  6356752.21424518
    #define e2_lla2ecef ((a_lla2ecef*a_lla2ecef - b_lla2ecef*b_lla2ecef)/(a_lla2ecef*a_lla2ecef))
    
    llh.f.x = llh.f.x * PI/180.0;
    llh.f.y = llh.f.y * PI/180.0;

    double N = (a_lla2ecef/(sqrt(1 - e2_lla2ecef*sin(llh.f.x)*sin(llh.f.x))));
    
    ecef->f.x = (N + llh.f.z) * cos(llh.f.x) * cos(llh.f.y);
    ecef->f.y = (N + llh.f.z) * cos(llh.f.x) * sin(llh.f.y);
    ecef->f.z = ((((b_lla2ecef*b_lla2ecef)/(a_lla2ecef*a_lla2ecef))*N) + llh.f.z) * sin(llh.f.x);

}

void llh2eci(double jd_actual, vec3 llh, vec3 * eci)
{
    vec3 ecef;
    double gst;
    /* compute GST value */
    greenwidtchtime(jd_actual, &gst);
    llh2ecef(llh, &ecef);
    ecef2eci(gst, ecef, eci);
}

void jd2date(double jday, int *year, int *month, int *day, int *hour, int *minute, int *second)
{
    /* JD2DATE Gregorian calendar date from modified Julian day number.
     [YEAR, MONTH, DAY, HOUR, MINUTE, SECOND] = JD2DATE(JD) returns the
     Gregorian calendar date (year, month, day, hour, minute, and second)
     corresponding to the Julian day number JD.
     */
    
    int ijd, a, b, c, d, ee, m;
    double fjd;
    
    // Adding 0.5 to JD and taking FLOOR ensures that the date is correct.
    
    ijd = floor(jday + 0.5);                // integer part
    fjd = jday - ijd + 0.5;                 // fraction part
    
    (*second) = 86400 * fjd;
    (*hour)   = floor((*second) / 3600);              // get number of hours
    (*second) = (*second) - 3600 * (*hour);         // remove the hours
    (*minute) = floor((*second) / 60);                // get number of minutes
    (*second) = (*second) - 60 * (*minute);         // remove the minutes
    
    // The following algorithm is from the Calendar FAQ.
    
    a = ijd + 32044;
    b = floor((4 * a + 3) / 146097);
    c = a - floor((b * 146097) / 4);
    
    d = floor((4 * c + 3) / 1461);
    ee = c - floor((1461 * d) / 4);
    m = floor((5 * ee + 2) / 153);
    
    (*day)   = ee - floor((153 * m + 2) / 5) + 1;
    (*month) = m + 3 - 12 * floor(m / 10);
    (*year)  = b * 100 + d - 4800 + floor(m / 10);
}

/* Determine day count */
int month_cnt (int month)
{
    int count = 0;
    
    if (month == 1) count= 0;
    else if (month == 2) count = 31;
    else if (month == 3) count = 60;//59; 2016 is leap year
    else if (month == 4) count = 90;
    else if (month == 5) count = 120;
    else if (month == 6) count = 151;
    else if (month == 7) count =181;
    else if (month == 8) count = 212;
    else if (month == 9) count = 243;
    else if (month == 10) count = 273;
    else if (month == 11) count = 304;
    else if (month == 12) count = 334;
    
    return count;
}

/*Determine day count of the year*/
void dayofyear (int month, int dayofmonth, int *dyear)
{
    int day_count;
    day_count = month_cnt(month);
    
    *dyear = dayofmonth + day_count;
}

double j_day( unsigned int unixSecs )
{
    return ( unixSecs / 86400.0 ) + 2440587.5;
}


void unixtime2decyear(double unixSec, double *dec_year)
{
    int second, minute, hour, day, month, year, day_year;
    double julianday;
    
    julianday = j_day(unixSec);
    jd2date(julianday, &year, &month, &day, &hour, &minute, &second);
    
    dayofyear(month, day, &day_year);
    
    *dec_year = year + (double)day_year/366.0;
}

void greenwidtchtime(double jd, double *gst)
{
    double temp, tut1;

    // ------------------------  implementation   ------------------
    tut1 = ( jd - 2451545.0 ) / 36525.0;

    temp = - 6.2e-6 * tut1 * tut1 * tut1 + 0.093104 * tut1 * tut1  + (876600.0 * 3600.0 + 8640184.812866) * tut1 + 67310.54841;

    // 360/86400 = 1/240, to deg, to rad

    temp = fmod(temp * PI180 / 240.0, TWOPI);

    // ------------------------ check quadrants --------------------
    if ( temp < 0.0 )
    {
        temp = temp + TWOPI;
    }

    (*gst) = temp;
}

int sat_position_eci(double jd_actual, vec3 *position_eci, vec3 *velocity_eci)
{
    xyz_t pos, vel;
    int error;
    
    error = satpos_xyz(jd_actual, &pos, &vel);
    
    position_eci->raw[0] = pos.x; /* Position in km's */
    position_eci->raw[1] = pos.y;
    position_eci->raw[2] = pos.z;
    
    velocity_eci->raw[0] = vel.x; /* Velocity in km/s */
    velocity_eci->raw[1] = vel.y;
    velocity_eci->raw[2] = vel.z;
    
    return error;
}


int actualposition(double jd_actual, tle_set * tle, vec3 *sat_eci, vec3 *sat_vel)
{
    orbit_t orb;
    long satno = 0;
    
    vec3 pos_eci, vel_eci;
    vec3 pos_ecef, vel_ecef;
    vec3 xsat_ecef, xsat_eci, vsat_eci, vsat_ecef;

    double gst;
    /* compute GST value */
    greenwidtchtime(jd_actual, &gst);
    
    /* TLE to ORBIT PARAMETERS */
    read_twoline(tle->lines._1, tle->lines._2, satno, &orb);
    
    if(init_sgdp4(&orb) == SGDP4_ERROR)
    {
        printf("Error from init_sgdp4()");
        return 1;
    }
    
    /* satellite position in eci [kms] */
    if(sat_position_eci(jd_actual, &pos_eci, &vel_eci) == SGDP4_ERROR)
    {
        printf("\n\tError from sat_position() \n");
        return 1;
    }
    /* Position: km's to meters */
    mult_vec3_scalar(pos_eci, KM2M, sat_eci);
    mult_vec3_scalar(vel_eci, KM2M, sat_vel);
    /* Transformation to Latitude(deg), Longitude(deg) and Height(m) */
    return 0;
}

