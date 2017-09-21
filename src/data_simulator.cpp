#include "data_simulator.h"
#include <cstdio>
#include <iostream>
#include <cmath>

DataSimulator::DataSimulator()
{
    /* nothing i thing */
}

void DataSimulator::setPassDuration(unsigned int seconds)
{
    _pass_duration = seconds;
}

/* frames per X seconds */
void DataSimulator::setFrameInterval(double high_second)
{
    double intgrpart;
    double fractpart = modf(high_second, &intgrpart);
    if (fractpart > 0.25 && fractpart <= 0.5) {
        _frame_interval = intgrpart + 0.5;
    }else if (fractpart <= 0.25) {
        _frame_interval = intgrpart + 0.25;
    }else if (fractpart >= 0.5 && fractpart < 0.99) {
        _frame_interval = intgrpart + 0.5;
    }
    _frame_interval = (double) high_second;
}

void DataSimulator::setFrameSize(unsigned int len)
{
    _frame_size = len;
}

void DataSimulator::setMaxDataDutyCycle(unsigned int duty)
{
    _max_duty = duty;
}

void DataSimulator::addScientificRedundacy(double redundancy)
{
    double actual_frame_count, scientific_frame_count;
    _redundant_dl_time = ceil((1.0/redundancy) * _sci_dl_time);
    if (_redundant_dl_time >= _pass_duration) {
        actual_frame_count = (double)(_pass_duration/_frame_interval);
    }else {
        actual_frame_count = (double)(_sci_dl_time/_frame_interval);
    }
    scientific_frame_count = ceil((1.0/redundancy) * actual_frame_count);
    while (actual_frame_count < scientific_frame_count) {
        if (_sci_dl_time >= _pass_duration) {
            _frame_interval -= 0.25;
            actual_frame_count = (double)(_pass_duration/_frame_interval);
        }else {
            _sci_dl_time++;
            actual_frame_count = (double)(_sci_dl_time/_frame_interval);
        }
        while ((_frame_size/_frame_interval) > _max_duty) {
            _frame_size--;
        }
    }
    _redundant_dl_time = scientific_frame_count*_frame_interval;
}

void DataSimulator::addMinimumLogInfo(unsigned int info)
{
    unsigned int start_frame_size;
    start_frame_size = _frame_size;
    while (getLogMessageSize() < info) {
        _frame_interval -= 0.25;
        while ((_frame_size/_frame_interval) > _max_duty) {
            _frame_size--;
        }
    }
}

void DataSimulator::setScientificSize(unsigned int scientific_kb)
{
    _sci_kb = scientific_kb;
}

void DataSimulator::setScientifcDLTime(unsigned long dl_time)
{
    _sci_dl_time = dl_time;
    _redundant_dl_time = dl_time;
}

void DataSimulator::setTelemetryMessageSize(unsigned int telemetry_kb)
{
    _tel_kb = telemetry_kb;
}

unsigned int DataSimulator::getScientificBlockSize()
{
    calculatePassBudget();
    if ((getPassBudget()) > _sci_kb) {
        return ((unsigned int) floor((float)((_sci_kb * _frame_interval)/_sci_dl_time)));
    }else {
        return 0;
    }
}

unsigned int DataSimulator::getLogMessageSize()
{
    return getLogBlockSize() * (_pass_duration/_frame_interval);
}

unsigned int DataSimulator::getLogBlockSize()
{
    return (_frame_size - (getScientificBlockSize() + _tel_kb));
}

void DataSimulator::calculatePassBudget()
{
    _pass_budget = (_frame_size * (_pass_duration/_frame_interval));
}

unsigned int DataSimulator::getPassBudget()
{
    return _pass_budget;
}

double DataSimulator::getTelemetryPeriod()
{
    return (_frame_interval);
}

double DataSimulator::getFrameInterval()
{
    return (_frame_interval);
}

unsigned int DataSimulator::getFrameSize()
{
    return (_frame_size);
}

unsigned int DataSimulator::getPayloadDLTime()
{
    return (_redundant_dl_time);
}

void DataSimulator::plotCurrentBudget()
{
    std::cout << _pass_duration << ", ";
    std::cout << getFrameInterval() << ", ";
    std::cout << getFrameSize() << ", ";
    //std::cout << getPayloadDLTime() << ", ";
    std::cout << getPassBudget() << ", ";
    std::cout << getLogMessageSize()/1024.0 << ", ";
    //std::cout << getScientificBlockSize() << ", ";
    std::cout << getLogBlockSize() << std::endl;
}
