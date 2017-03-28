#include "payload_tracker_api.h"

static tle_set * load_tle_from_file(FILE *fp)
{
    /* Each TLE is a 3 line set, where want to stract the satellite ID, and the 2 lines, to compute its position */
    if (fp == NULL){
        _api_printf(1, "Error openning file\n");
        return NULL;
    }
    char line[256];
    int first = 1;
    /* The first read must be 24 characters */
    tle_set * tle = malloc(sizeof(tle_set));
    if (tle == NULL){
        return NULL;
    }
    while(fgets(line, sizeof(line), fp) != NULL){
        /* Get endlines out */
        if (strlen(line) <= TLE_NAME_SIZE + 2){
            if (strlen(line) > TLE_NAME_SIZE){
                strncpy(tle->lines._name, line, TLE_NAME_SIZE);
                tle->lines._name[TLE_NAME_SIZE] = '\0';
            }else{
                strncpy(tle->lines._name, line, strlen(line) - 1);
                tle->lines._name[TLE_NAME_SIZE] = '\0';
            }
        }else{
            if (first){
                if (strlen(tle->lines._name) == 0){
                    strcpy(tle->lines._name, "DUMMYsat");
                    tle->lines._name[TLE_NAME_SIZE] = '\0';   
                }
                if (line[0] != '1'){
                    if (tle)
                        free(tle);
                    return NULL;
                }
                strncpy(tle->lines._1, line, TLE_LINE_SIZE);
                first = 0;
            }else{
                if (line[0] != '2'){
                    if (tle)
                        free(tle);                    
                    return NULL;
                }
                strncpy(tle->lines._2, line, TLE_LINE_SIZE);
                return tle;
            }
        }
    }
    if (tle)
        free(tle);
    return NULL;
   /* Returns 0 at EOF, -1 at error, and > 0 as the pointer of the file */
}

static int get_receiver_motion(double jd, const char * tle_file, vec3 * llh_pos, object_motion_t * receiver)
{
    FILE * fp;
    tle_set * tle;
    if (receiver == NULL){
        _api_printf(1, "Receiver motion is null \n");
    }
    if (tle_file == NULL && llh_pos == NULL){
        _api_printf(1, "Really, you are stupid\n");
        return -1;
    }
    if (tle_file != NULL){
        /* Process TLE file */
        /* If there are both, just tle is preferred */
        fp = fopen(tle_file, "r");
        if (fp == NULL){
            return -1;
        }
        tle = load_tle_from_file(fp);
        if (tle == NULL){
            return -1;
        }

        if (actualposition(jd, tle, &receiver->pos, &receiver->vel) == 0){
            /* Which is that position? */
            free(tle);
            fclose(fp);
            return 0;
        }else{
            free(tle);
            fclose(fp);
            return -1; 
        }

    }
    if (llh_pos != NULL){
        llh2eci(jd, *llh_pos, &receiver->pos);
        receiver->vel.f.x = 0;
        receiver->vel.f.y = 0;
        receiver->vel.f.z = 0;
        return 0;
    }
}

/* pos1 is the target, pos2 is you */
static double get_range_rate(vec3 pos1, vec3 pos2, vec3 vel1, vec3 vel2)
{
    /* Range */
    vec3 delta_vel, delta_pos;
    double range = sqrt (pow((pos1.f.x - pos2.f.x), 2) + pow((pos1.f.y - pos2.f.y), 2) + pow((pos1.f.z - pos2.f.z), 2) );

    sub_vec3(vel1, vel2, &delta_vel);
    sub_vec3(pos1, pos2, &delta_pos);

    /* target calc */
    return ( (delta_pos.f.x*delta_vel.f.x + delta_pos.f.y*delta_vel.f.y + delta_pos.f.z*delta_vel.f.z) / range );
}

static double get_doppler(double frequency, double range_rate)
{
    #define Light_Speed 299792458.0
    return ((frequency * (1.0 - (range_rate / Light_Speed))) - frequency);
}

/* Input 2 object motions */
/* Output Azimuth, Elevation, Distance */
static int get_payload_pointing(double jd, object_motion_t * payload, object_motion_t * ground, payload_location_t * output)
{
    vec3 p_vector;
    if (payload == NULL || ground == NULL || output == NULL){
        printf("NULL object at %s\n", __FUNCTION__);
        return -1;
    }
    /* Get elevation angle and azimuth angle */
    /* Elevation is 90-angle between both */
    azimuth_elev_dist_vec3(jd, payload->pos, ground->pos, &p_vector);
    output->az      = p_vector.f.x;
    output->el      = p_vector.f.y;
    output->dist    = p_vector.f.z;
    return 0;
    /* We have the pointing vector now */
}

/* Function wrapper for all the functions, you input a UNIX TIMESTAMP, and a configuration struct */
/* Input for the Station is LLH or TLE */
/* Input for the Platform is LLH or TLE */
/* Input frequency (for doppler est) */
/* Output Platform LLH, Station LLH */
/* Output Doppler Estimation (if freq is given), Relative Speed, Relative Distance respect to Station */ 
/* Output Azimuth and Elevation respect to Station */

int propagate_from_station(propagation_config_t * conf, propagation_output_t * res)
{
    object_motion_t platform;
    object_motion_t station;
    payload_location_t out;
    
    if (conf == NULL || res ==  NULL){
        return -1;
    }
    if (conf->station_has_tle){
        if (get_receiver_motion(j_day(conf->timestamp), conf->station_tle, NULL, &station) != 0){
            return -1;
        }
        eci2llh(j_day(conf->timestamp), station.pos, &res->station_llh);
    }else{
        if (get_receiver_motion(j_day(conf->timestamp), NULL, &conf->station_llh, &station) != 0){
            return -1;
        }
        memcpy(&res->station_llh, &conf->station_llh, sizeof(vec3));
    }
    if (conf->platform_has_tle){
        if (get_receiver_motion(j_day(conf->timestamp), conf->platform_tle, NULL, &platform) != 0){
            return -1;
        }
        eci2llh(j_day(conf->timestamp), platform.pos, &res->platform_llh);
    }else{
        if (get_receiver_motion(j_day(conf->timestamp), NULL, &conf->platform_llh, &platform) != 0){
            return -1;
        }
        memcpy(&res->platform_llh, &conf->platform_llh, sizeof(vec3));
    }
    /* Now we have all motions */
    if (get_payload_pointing(j_day(conf->timestamp), &platform, &station, &out) != 0){
        return -1;
    }
    res->az = out.az;
    res->el = out.el;
    res->rel_dist = out.dist;
    res->rel_velocity = get_range_rate(platform.pos, station.pos, platform.vel, station.vel);
    res->doppler = get_doppler(conf->in_freq, res->rel_velocity);
    return 0;
}

int main(void)
{
    int i;
    vec3 station_llh;
    vec3 platform_llh;
    propagation_config_t conf;
    propagation_output_t out;
    conf.station_has_tle = false;
    conf.platform_has_tle = true;
    station_llh.f.x = 41;
    station_llh.f.y = 2;
    station_llh.f.z = 100;
    memcpy(&conf.station_llh, &station_llh, sizeof(vec3));
    strcpy(conf.platform_tle, "mine_tle.txt");
    conf.in_freq = 100e6;
    conf.timestamp = time(NULL);
    for (i = 0; i < 20; i++){
        conf.timestamp += 240;
        propagate_from_station(&conf, &out);
        printf("%f, %f\n", out.platform_llh.raw[0], out.platform_llh.raw[1]);      
    }
    /*
    printf("Station LLH: %f º %f º %f km\n", out.station_llh.raw[0], out.station_llh.raw[1], out.station_llh.raw[2]/1000.0);
    printf("Platform LLH: %f º %f º %f km\n", out.platform_llh.raw[0], out.platform_llh.raw[1], out.platform_llh.raw[2]/1000.0);
    printf("Azimuth: %f º Elevation: %f º Distance: %f km\n", out.az, out.el, out.rel_dist/1000.0);
    printf("Relative Velocity: %f km/s\n", out.rel_velocity/1000.0);
    printf("Doppler from %f MHz -> %f Hz\n", conf.in_freq/1e6, out.doppler);
    */
    return 0;
}