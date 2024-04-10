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
static uint32_t time_delta_turn_us;

/**** Public API ****/

float get_pedaling_speed(void)
{
    /* Between two magnets the crankset has rotated of 1.0/SENSOR_MAGNET_COUNT
       turn and it took time_delta microseconds. So we can calculate te current
       pedaling speed :
       RPM = 1 / (time_delta_magnet_minute * SENSOR_MAGNET_COUNT)
    */
    return 1.0F / (US_TO_MIN((float)time_delta_magnet_us) * (float)SENSOR_MAGNET_COUNT);
}

float get_average_pedaling_speed(void)
{
    /* The average pedaling speed on the previous turn is simply
       the inverse of the time (in minute) elapased during the last turn
    */
    return 1.0F / US_TO_MIN((float)time_delta_turn_us);
}

void new_magnet_cb(void)
{
    static uint8_t magnet_count = 0U;
    static uint32_t timestamps[SENSOR_MAGNET_COUNT] = {0UL};
    static uint32_t previous_timestamp = 0UL;
    uint32_t oldest_timestamp = 0UL;
    uint32_t current_timestamp = 0UL;

    /* Circular buffer to compute average pedaling speed */
    if(magnet_count >= (SENSOR_MAGNET_COUNT - 1U))
    {
        oldest_timestamp = timestamps[0];
        magnet_count = 0U;
    }
    else
    {
        magnet_count++;
        oldest_timestamp = timestamps[magnet_count];
    }

    current_timestamp = get_timestamp();

    /* Store the current timestamp for next turn computation */
    timestamps[magnet_count] = current_timestamp;

    /* Compute elapsed time since last magnet detection */
    if(current_timestamp < previous_timestamp) /* timestamp overflow */
    {
        time_delta_magnet_us = UINT32_MAX - previous_timestamp + current_timestamp;
    }
    else
    {
        time_delta_magnet_us = current_timestamp - previous_timestamp;     
    }

    /* Compute elapsed time during the last full crankset turn */
    if(current_timestamp < oldest_timestamp) /* timestamp overflow */
    {
        time_delta_turn_us = UINT32_MAX - oldest_timestamp + current_timestamp;
    }
    else
    {
        time_delta_turn_us = current_timestamp - oldest_timestamp;
    }

    /* Store previous timestamp for next magnet computation */
    previous_timestamp = current_timestamp;
}
