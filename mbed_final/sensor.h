#ifndef SENSOR_H
#define SENSOR_H

#include "all_headers.h"

class Sensor {
    private:
        events::EventQueue &_event_queue;
        
    public:
        Sensor(events::EventQueue &event_queue);
        void getData();
        void init_params();

        int16_t _pAccDataXYZ[3] = {0};
        float _pGyroDataXYZ[3] = {0};
        int8_t itemFront;
        int8_t itemBack;
        int8_t acc;
};

#endif