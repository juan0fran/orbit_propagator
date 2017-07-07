#ifndef __DATA_SIM_HPP__
#define __DATA_SIM_HPP__

class DataSimulator {

    public:
        DataSimulator();

        /* data available per orbit */
        void setPassDuration(unsigned int seconds);
        /* frames per X seconds */
        void setFrameInterval(double high_second);

        void setMaxDataDutyCycle(unsigned int duty);
        void calculatePassBudget();
        /* amount of size per data source */
        /* this needs to be downloaded in a unique pass */
        void setScientificSize(unsigned int scientific_kb);
        void setScientifcDLTime(unsigned long dl_time);
        void addScientificRedundacy(double redundancy);
        void addMinimumLogInfo(unsigned int info);
        /* this is best effort */
        void setTelemetryMessageSize(unsigned int telemetry_kb);
        void setFrameSize(unsigned int len);

        unsigned int getScientificBlockSize();
        unsigned int getLogBlockSize();
        unsigned int getLogMessageSize();

        double       getTelemetryPeriod();
        unsigned int getPassBudget();

        unsigned int getFrameSize();
        double       getFrameInterval();
        unsigned int getPayloadDLTime();

        void plotCurrentBudget();

    private:
        unsigned int _pass_budget;
        unsigned int _pass_duration;

        unsigned int _frame_size;
        double       _frame_interval;

        unsigned int _scientific_frame_count;

        unsigned int _sci_kb;
        unsigned int _sci_dl_time;
        unsigned int _redundant_dl_time;

        unsigned int _log_kb;
        unsigned int _tel_kb;

        unsigned int _max_duty;
};

#endif
