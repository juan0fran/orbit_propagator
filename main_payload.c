#include "payload_tracker_api.h"
#include <unistd.h>

void get_llh()
{
    vec3 single_object;
    propagation_config_t single_conf;

    single_conf.timestamp = time(NULL);
    single_conf.has_tle = true;
    strcpy(single_conf.tle, "mine_tle.txt");

    propagate_and_get_llh(&single_conf, &single_object);
    printf("Propagating one: %f, %f\n", single_object.f.x, single_object.f.y);
}

void get_range()
{
    vec3 start_point;
    vec3 end_point;

    propagation_config_t single_conf;
    square_region_t region;
    struct tm ts;
    char buf[80];

    time_t start_time;
    time_t end_time;

    bool start_not_found = true;
    bool end_not_found = true;

    single_conf.timestamp = time(NULL);
    single_conf.has_tle = true;
    strcpy(single_conf.tle, "mine_tle.txt");

    region.p1.f.x = 0.0;
    region.p1.f.y = 150.0;

    region.p2.f.x = 0.0;
    region.p2.f.y = -160.0;

    region.p3.f.x = 45.0;
    region.p3.f.y = 150.0;

    region.p4.f.x = 45.0;
    region.p4.f.y = -160.0;

    int points_found = 0;
    while(points_found++ < 20) {
        start_not_found = true;
        end_not_found = true;
        while(start_not_found == true) {
            propagate_and_get_llh(&single_conf, &start_point);
            /* we need to be in the same quadrant or boundaries */
            if (ispoint_inside_region(start_point, region) == true) {
                start_not_found = false;
                start_time = single_conf.timestamp;
            }
            single_conf.timestamp += 1;
        }
        ts = *localtime(&start_time);
        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

        //printf("We will be on the margin at time: %s, ", buf);
        //printf("Coords: LAT/LON: %f,%f\n", single_object.f.x, single_object.f.y);

        single_conf.timestamp = start_time;
        while(end_not_found == true) {
            propagate_and_get_llh(&single_conf, &end_point);
            if (ispoint_inside_region(end_point, region) == false) {
                end_not_found = false;
                end_time = single_conf.timestamp;
            }
            single_conf.timestamp += 1;
        }
        ts = *localtime(&end_time);
        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

        if ((end_time - start_time) > 60*10) {
            printf("%f,%f\n", start_point.f.y, start_point.f.x);
            printf("%f,%f\n", end_point.f.y, end_point.f.x);
        }
        //printf("We will be out of the margin at time: %s, ", buf);
        //printf("Coords: LAT/LON: %f,%f\n", single_object.f.x, single_object.f.y);
        //printf("This shit gives: %d minutes and %d seconds of payload\n", (int)(end_time-start_time)/60, (int)(end_time-start_time)%60);
    }
}

/* Get visibility test */
void get_visibility()
{
    int i;
    vec3 station_llh;
    vec3 platform_llh;
    visibility_config_t conf;
    propagation_output_t out;
    conf.station.has_tle = false;
    conf.platform.has_tle = true;
    station_llh.f.x = 32.18;
    station_llh.f.y = -96.88;
    station_llh.f.z = 10;
    time_t now;
    struct tm ts;
    char buf[80];

    memcpy(&conf.station.llh, &station_llh, sizeof(vec3));
    strcpy(conf.platform.tle, "mine_tle.txt");
    conf.in_freq = 100e6;
    conf.station.timestamp = time(NULL);
    conf.platform.timestamp = time(NULL);
    int delta_time = 5;
    int minutes_of_simulation = 120;
    int count_simulation = minutes_of_simulation * (60 / delta_time);

    for (i = 0; i < count_simulation; i++) {
        propagate_and_get_visibility(&conf, &out);
        if (out.el >= 0) {
            now = conf.station.timestamp;
            ts = *localtime(&now);
            strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
            printf("Time: %s, ", buf);
            printf("Az: %f, EL: %f, ", out.az, out.el);
            printf("LAT: %f, LON: %f\n", out.platform_llh.raw[0], out.platform_llh.raw[1]);
        }
        conf.station.timestamp += delta_time;
        conf.platform.timestamp += delta_time;
    }
}

int main(int argc, char ** argv)
{
    if (argc > 2) {
        get_visibility();
    }else if (argc == 1) {
        get_range();
    }else {
        get_llh();
    }
    return 0;
}
