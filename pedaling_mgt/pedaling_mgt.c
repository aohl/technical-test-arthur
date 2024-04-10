/**
 * Copyright (c) 2019 eBikeLabs. All Rights Reserved.
 */

#include "pedaling_mgt.h"

#include "time_mgt.h"

/**** Defines ****/

#define SENSOR_MAGNET_COUNT 12U
#define US_TO_MIN(x)        ((float)x/60e6)

/**** Public variables ****/

static uint32_t time_delta_magnet_us;

/**** Public API ****/

float get_pedaling_speed(void)
{
    /* Between two magnets the crankset has rotated of 1.0/SENSOR_MAGNET_COUNT turn
       and it took time_delta microseconds. So we can calculate te current pedaling speed :
       RPM = 1 / (time_delta_magnet_minute * SENSOR_MAGNET_COUNT) 
    */
    return 1.0F / (US_TO_MIN((float)time_delta_magnet_us) * (float)SENSOR_MAGNET_COUNT);
}

float get_average_pedaling_speed(void)
{
    // WRITE YOUR CODE HERE
    return 0.f;
}

void new_magnet_cb(void)
{
    static uint32_t previous_timestamp = 0UL;
    uint32_t current_timestamp = 0UL;

    current_timestamp = get_timestamp();

    /* Compute elapsed time since last magnet detection */
    if(current_timestamp < previous_timestamp) /* timestamp overflow */
    {
        time_delta_magnet_us = UINT32_MAX - previous_timestamp + current_timestamp;
    }
    else
    {
        time_delta_magnet_us = current_timestamp - previous_timestamp;     
    }

    /* Store previous timestamp for next magnet computation */
    previous_timestamp = current_timestamp;
}
