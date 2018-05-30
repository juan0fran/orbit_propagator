#include <cstdio>
#include <iostream>
#include <fstream>
#include <locale>

#include "orbit_simulator.h"

OrbitSimulator::OrbitSimulator()
{
    /* we have TLE for spacecraft, but LLH for station */
    _orbit_conf.station.has_tle = false;
    _orbit_conf.platform.has_tle = true;
    _sim_timestep = 1;
    _min_elev = 0.0;
}

OrbitSimulator::~OrbitSimulator() {};

void OrbitSimulator::SetMinimumElevation(double elev)
{
    _min_elev = elev;
}

void OrbitSimulator::SetCommsFreq(double freq)
{
    _orbit_conf.in_freq = freq;
    Link::setFrequency(freq);
}

void OrbitSimulator::SetGroundLocation(double lat, double lon, double height)
{
    _orbit_conf.station.llh.f.x = lat;
    _orbit_conf.station.llh.f.y = lon;
    _orbit_conf.station.llh.f.z = height;
}

void OrbitSimulator::SetSpaceTLEFile(const char *tle_file)
{
    strcpy(_orbit_conf.platform.tle, tle_file);
}

void OrbitSimulator::SetTimestep(unsigned long timestep)
{
    _sim_timestep = timestep;
}

void OrbitSimulator::SetSimulationInterval(unsigned long start, unsigned long end)
{
    _sim_start          = start;
    _sim_end            = end;
    _npoints_forecast   = ((end-start) / _sim_timestep);
    _orbit_conf.station.timestamp = start;
    _orbit_conf.platform.timestamp = start;
}

void OrbitSimulator::SetSimulationInterval(unsigned long start, unsigned long end, unsigned int timestep)
{
    _sim_start          = start;
    _sim_end            = end;
    _sim_timestep       = timestep;
    _npoints_forecast   = ((end-start) / _sim_timestep);
    _orbit_conf.station.timestamp = start;
    _orbit_conf.platform.timestamp = start;
}

std::string OrbitSimulator::GetResults()
{
    std::stringstream str;
    str.imbue(std::locale(""));
    for (SimulatorResultsVec::iterator it = Results.begin(); it != Results.end(); it++) {
        if (it->propagation.el >= _min_elev) {
            str << it->propagation.timestamp << ";";
            //str << it->propagation.az << ";";
            str << it->propagation.el << ";";
            //str << it->propagation.ul_doppler << ";";
            //str << it->propagation.dl_doppler << std::flush << std::endl;
            str << it->propagation.rel_dist <<  ";";
            str << it->link.snr << ";";
            str << it->link.ber << std::flush << std::endl;
            //std::cout << it->link.per << "; ";
            //std::cout << it->link.rfer <<  "; ";
            //std::cout << it->link.bfer << std::endl;
        }
    }
    return str.str();
}

unsigned long OrbitSimulator::getLastPassDuration()
{
    return (getLastFoundPassEnd() - getLastFoundPassStart());
}

unsigned long OrbitSimulator::getLastFoundPassStart()
{
    return _last_pass_start;
}

unsigned long OrbitSimulator::getLastFoundPassEnd()
{
    return _last_pass_end;
}

void OrbitSimulator::findNextPass(unsigned long start_timestamp)
{
    bool passFound = false;
    SimulatorResults single_result;
    SetSimulationInterval(start_timestamp, start_timestamp + 86400);
    Results.reserve(_npoints_forecast);
    for (unsigned long t = _sim_start; t < _sim_end; t = t + _sim_timestep) {
        _orbit_conf.station.timestamp   = t;
        _orbit_conf.platform.timestamp  = t;
        if (propagate_and_get_visibility(&_orbit_conf, &single_result.propagation) == -1 ) {
            std::cout << "Error in simulation" << std::endl;
            Results.shrink_to_fit();
            return;
        }
        if (single_result.propagation.el < _min_elev) {
            if (passFound == true) {
                _last_pass_end = t;
                if ((_last_pass_end - _last_pass_start) < 1) {
                    Results.clear();
                    SetSimulationInterval(t+_sim_timestep, t+_sim_timestep + 86400);
                    passFound = false;
                    continue;
                }
                Results.shrink_to_fit();
                return;
            }
        }else {
            if (passFound == false) {
                _last_pass_start = t;
                passFound = true;
            }
            setDistance(single_result.propagation.rel_dist);
            fillStructure(&single_result.link);
            Results.push_back(single_result);
        }
    }
}

unsigned long OrbitSimulator::getPassAvailability()
{
    int count_good = 0, count_bad = 0;
    for (SimulatorResultsVec::iterator it = Results.begin(); it != Results.end(); it++) {
        if (it->link.ber > 1e-5) {
            count_bad++;
        }else {
            count_good++;
        }
    }
    return (count_good*100)/(count_good+count_bad);
}
