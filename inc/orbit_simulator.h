#ifndef __ORBIT_SIMULATOR_HPP__
#define __ORBIT_SIMULATOR_HPP__

#include <string>
#include <vector>
#include "link.h"
#include "payload_tracker_api.h"

typedef struct SimulatorResults {
    propagation_output_t propagation;
    LinkResults link;
}SimulatorResults;

typedef std::vector<SimulatorResults> SimulatorResultsVec;

class OrbitSimulator : public Link{
    public:
        OrbitSimulator();
        ~OrbitSimulator();

        void SetMinimumElevation(double elev);
        void SetCommsFreq(double freq);
        void SetGroundLocation(double lat, double lon, double height);
        void SetSpaceTLEFile(const char *tle);
        void SetTimestep(unsigned long timestep);
        
        void SetSimulationInterval(unsigned long start, unsigned long end);
        void SetSimulationInterval(unsigned long start, unsigned long end, unsigned int timestep);

        std::string GetResults();

        unsigned long getLastFoundPassStart();
        unsigned long getLastFoundPassEnd();
        unsigned long getLastPassDuration();
        unsigned long getPassAvailability();

        void run();
        void findNextPass(unsigned long start_timestamp);

        SimulatorResultsVec   Results;

    private:
        visibility_config_t     _orbit_conf;
        double                  _min_elev;
        unsigned long           _sim_start;
        unsigned long           _sim_end;
        unsigned long           _sim_timestep;
        unsigned int            _npoints_forecast;

        unsigned long           _last_pass_start;
        unsigned long           _last_pass_end;

};

#endif
