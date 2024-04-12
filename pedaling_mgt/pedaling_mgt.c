/**
 * Copyright (c) 2019 eBikeLabs. All Rights Reserved.
 */

#include "pedaling_mgt.h"

#include "time_mgt.h"

/**** Private defines ****/

#define SENSOR_MAGNET_COUNT               12U  /* Number of magnets per crankset turn */
#define AVERAGE_RESET_TIME_THRESHOLD_US   1e6  /* Reset average after 1 second of no pedaling */

#define US_TO_MIN(x)        ((float)(x)/60.0e6F)

/**** Private variables ****/

static float pedaling_speed_rpm = 0.0F;
static float pedaling_speed_average_rpm = 0.0F;

/**** Public API ****/

float get_pedaling_speed(void)
{
    return pedaling_speed_rpm;
}

float get_average_pedaling_speed(void)
{
    return pedaling_speed_average_rpm;
}

void new_magnet_cb(void)
{
    static uint8_t start_magnet_index = 0U;
    static uint8_t magnet_index = 0U;
    static uint32_t timestamps[SENSOR_MAGNET_COUNT] = {0UL};
    static uint32_t previous_timestamp = 0UL;
    uint32_t oldest_timestamp;
    uint32_t current_timestamp;

    /* Get current timestamp */
    current_timestamp = get_timestamp();

    /* if duration since last pedal stroke exeed thresold, reset indexes */
    if((current_timestamp - previous_timestamp) > AVERAGE_RESET_TIME_THRESHOLD_US)
    {
        start_magnet_index = 0U;
        magnet_index = 0U;
    }

    /* Instantaneous pedaling speed computation for first magnet detection (force to 0.0 RPM) */
    if(start_magnet_index == 0U)
    {
        pedaling_speed_rpm = 0.0F;
    }
    else /* Instantaneous pedaling speed computation in general case */
    {
        pedaling_speed_rpm = 1.0F / ((float)SENSOR_MAGNET_COUNT * US_TO_MIN((float)current_timestamp - (float)previous_timestamp));
    }

    /* Pedaling speed average management for first crankset turn */
    if(start_magnet_index < SENSOR_MAGNET_COUNT)
    {
        /* Oldest timestamp is the first recorded timestamp */
        oldest_timestamp = timestamps[0U];

        /* Average pedaling speed during the first turn is calculated on the portion of turn */
        pedaling_speed_average_rpm = (float)start_magnet_index / (US_TO_MIN((float)current_timestamp - (float)oldest_timestamp) * (float)SENSOR_MAGNET_COUNT);

        /* Count number of magnets since begining of stroke */
        start_magnet_index++;
    }
    else
    {
        /* Average pedaling speed in normal case is simply calculated with the difference of timestamp
           after one turn */
        oldest_timestamp = timestamps[magnet_index];
        /* If current < oldest (timestamp overflow) unsigned int substraction still
           gives the correct result (circular arithmetic) */
        pedaling_speed_average_rpm = 1.0F / US_TO_MIN((float)current_timestamp - (float)oldest_timestamp);
    }

    /* Store the current timestamp for next turn average pedaling speed computation */
    timestamps[magnet_index] = current_timestamp;

    /* Store previous timestamp for next magnet pedaling speed computation */
    previous_timestamp = current_timestamp;

    /* Circular index to know the relative position of the current magnet */
    magnet_index++;
    magnet_index %= SENSOR_MAGNET_COUNT;
}
