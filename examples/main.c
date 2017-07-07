#include "gps_tracker_api.h"
#include "galileo_tracker_api.h"

#define SAT_RECEIVER

int main (void)
{
   /* Julian day from timestamp */
    gps_constellation_t found_gps;
    galileo_constellation_t found_gal;

    galileo_search_parameters_t search_gal;
    gps_search_parameters_t search_gps;
    object_motion_t receiver;
    vec3 llh_rx;
    int i;
    /* Take care with mine_tle, should be freed at the end */
    #ifdef SAT_RECEIVER
    get_receiver_motion("mine_tle.txt", NULL, &receiver);
    #else 
    llh_rx.f.x = 41;
    llh_rx.f.y = 2;
    llh_rx.f.z = 100;
    get_receiver_motion(NULL, &llh_rx, &receiver);
    #endif

    strcpy(search_gps.tle_file, "gps-ops.txt");
    strcpy(search_gps.norad_to_prn_file, "norad_id_to_prn_gps.txt");
    search_gps.minimum_angle = 45.0;
    search_gps.max_satellite_amount = 5;
    search_gps.frequency = L1_FREQ;

    get_visible_gps_from_tle_file(&search_gps, &receiver, &found_gps);
    for (i = 0; i < GPS_CONSTELLATION_AMOUNT; i++){
        if (found_gps.found[i]){
            printf("Found GPS Satellite PRN %d, at Doppler: %f\n", i+1, found_gps.doppler[i]);
        }
    }

    strcpy(search_gal.tle_file, "galileo.txt");
    strcpy(search_gal.norad_to_prn_file, "norad_id_to_prn_galileo.txt");
    search_gal.minimum_angle = 45.0;
    search_gal.max_satellite_amount = 5;
    search_gal.frequency = L1_FREQ;
    
    get_visible_galileo_from_tle_file(&search_gal, &receiver, &found_gal);
    for (i = 0; i < GALILEO_CONSTELLATION_AMOUNT; i++){
        if (found_gal.found[i]){
            printf("Found GALILEO Satellite PRN %d, at Doppler: %f\n", i+1, found_gal.doppler[i]);
        }
    }

    return 0;
}
