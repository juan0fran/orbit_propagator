#include "gps_tracker_api.h"

#define SAT_RECEIVER

int main (void)
{
   /* Julian day from timestamp */
    gps_constellation_t found_gps;
    gps_search_parameters_t search;
    object_motion_t receiver;
    vec3 llh_rx;
    int ret, i;
    /* Take care with mine_tle, should be freed at the end */
    #ifdef SAT_RECEIVER
    get_receiver_motion("mine_tle.txt", NULL, &receiver);
    #else 
    llh_rx.f.x = 41;
    llh_rx.f.y = 2;
    llh_rx.f.z = 100;
    get_receiver_motion(NULL, &llh_rx, &receiver);
    #endif

    strcpy(search.tle_file, "gps-ops.txt");
    strcpy(search.norad_to_prn_file, "norad_id_to_prn.txt");
    search.minimum_angle = 45.0;
    search.max_satellite_amount = 5;
    search.frequency = L1_FREQ;

    get_visible_gps_from_tle_file(&search, &receiver, &found_gps);
    for (i = 0; i < GPS_CONSTELLATION_AMOUNT; i++){
        if (found_gps.found[i]){
            printf("Found Satellite PRN %d, at Doppler: %f\n", i+1, found_gps.doppler[i]);
        }
    }
    /* To be implemented */
    //get_visible_galileo_from_tle_file("gps-ops.txt", &receiver, 35.0);

    return 0;
}
