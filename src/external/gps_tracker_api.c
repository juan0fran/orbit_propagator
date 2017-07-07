#include "gps_tracker_api.h"

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

static void delta_vec3(vec3 s1, vec3 s2, vec3 *res)
{
    res->f.x = s1.f.x - s2.f.x;
    res->f.y = s1.f.y - s2.f.y;
    res->f.z = s1.f.z - s2.f.z;
}

/* pos1 is the target, pos2 is you */
static double get_range_rate(vec3 pos1, vec3 pos2, vec3 vel1, vec3 vel2)
{
    /* Range */
    vec3 delta_vel, delta_pos;
    double range = sqrt (pow((pos1.f.x - pos2.f.x), 2) + pow((pos1.f.y - pos2.f.y), 2) + pow((pos1.f.z - pos2.f.z), 2) );
    _api_printf(0, "Range: %f km, ", range/1000.0);

    delta_vec3(vel1, vel2, &delta_vel);
    delta_vec3(pos1, pos2, &delta_pos);

    /* target calc */
    return ( (delta_pos.f.x*delta_vel.f.x + delta_pos.f.y*delta_vel.f.y + delta_pos.f.z*delta_vel.f.z) / range );
}

static double get_doppler(double frequency, double range_rate)
{
    #define Light_Speed 299792458.0
    _api_printf(0, "Range rate: %f km/s, ", range_rate/1000.0);
    return ((frequency * (1.0 - (range_rate / Light_Speed))) - frequency);
}

static int get_norad_id(tle_set * tle)
{
    char num[32];
    if (tle == NULL){
        return -1;
    }
    if (strncpy(num, tle->lines._1+2, 5) == NULL){
        return -1;
    }
    return (atoi(num));
}

int get_receiver_motion(const char * tle_file, vec3 * llh_pos, object_motion_t * receiver)
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

        if (actualposition(j_day(time(NULL)), tle, &receiver->pos, &receiver->vel) == 0){
            /* Which is that position? */
            vec3 llh;
            eci2llh(j_day(time(NULL)), receiver->pos, &llh);
            _api_printf(1, "Receiver position: %fº %fº %f meters\n", llh.f.x, llh.f.y, llh.f.z);
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
        llh2eci(j_day(time(NULL)), *llh_pos, &receiver->pos);
        receiver->vel.f.x = 0;
        receiver->vel.f.y = 0;
        receiver->vel.f.z = 0;
        return 0;
    }
    return -1;
}

static int get_prn_from_norad(const char * norad_file, int norad_id)
{
    FILE *fp;
    char line[NORMAL_BUFFER_SIZE];
    char key[SHORT_BUFFER_SIZE];
    char value[SHORT_BUFFER_SIZE];
    if (norad_file == NULL){
        _api_printf(1, "File is NULL\n");
        return -1;
    }
    if ( (fp = fopen(norad_file, "r") ) == NULL){
        _api_printf(1, "Error openning file\n");
        return -1;
    }
    /* Get key,value pair */
    while (fgets(line, sizeof(line), fp) != NULL){
        sscanf(line, "%[^,] %*[,] %[^\n]", key, value);
        if (atoi(key) == norad_id){
            return (atoi(value));
        }
    }
    return 0;
}

int assert_gps_parms(gps_search_parameters_t * parms)
{
    if (parms == NULL){
        return 0;
    }
    if (strlen(parms->tle_file) <= 0){
        return 0;
    }
    if (strlen(parms->norad_to_prn_file) <= 0){
        return 0;
    }
    if (parms->minimum_angle > 90.0 || parms->minimum_angle <= 0.0){
        return 0;
    }
    if (parms->max_satellite_amount <= 0 || parms->max_satellite_amount > GPS_CONSTELLATION_AMOUNT){
        return 0;
    }
    /* Do not check frequency...*/
    return 1;
}


int get_visible_gps_from_tle_file(gps_search_parameters_t * gps_search, object_motion_t * receiver, gps_constellation_t * res)
{
    FILE * fp;
    tle_set * tle;
    object_motion_t gps;
    int found_prn;
    double doppler_estimation, angle;
    int satellite_count = 0;
    
    if (!assert_gps_parms(gps_search)){
        printf("Parameters introduced are invalid\n");
        return -1;
    }

    fp = fopen(gps_search->tle_file, "r");
    if (fp == NULL){
        _api_printf(1, "Error openning file\n");
        return -1;
    }
    memset(res->found, 0, sizeof(res->found));
    do{
        tle = load_tle_from_file(fp);
        if (tle != NULL){
            if (actualposition(j_day(time(NULL)), tle, &gps.pos, &gps.vel) == 0){
                angle = angle_vec3(gps.pos, receiver->pos);
                if (angle < gps_search->minimum_angle){
                    found_prn = get_prn_from_norad(gps_search->norad_to_prn_file, get_norad_id(tle));
                    doppler_estimation = get_doppler(gps_search->frequency, get_range_rate(gps.pos, receiver->pos, gps.vel, receiver->vel));
                    _api_printf(0, "Satellite Norad ID: %d, PRN: %d with Doppler: %f Hz\n", 
                            get_norad_id(tle), found_prn, doppler_estimation);
                    res->found[found_prn] = 1;
                    res->doppler[found_prn] = doppler_estimation;
                    satellite_count++;
                    if (satellite_count >= gps_search->max_satellite_amount){
                        return 0;
                    }
                }
            }
            free(tle);
        }
    }while(tle != NULL);
    fclose(fp);
    return 0;
}
