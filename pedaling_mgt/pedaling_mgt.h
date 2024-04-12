/**
 * Copyright (c) 2019 eBikeLabs. All Rights Reserved.
 */

#pragma once

/**** Public API ****/

/**
 * @brief Return current pedaling speed in RPM
 */
float get_pedaling_speed(void);

/**
 * @brief Return average current pedaling speed in RPM
 *
 * @note Average is resetted after AVERAGE_RESET_TIME_THRESHOLD_US microseconds without pedaling
 */
float get_average_pedaling_speed(void);

/**
 * @brief Callback when a magnet passes in front of the pedaling sensor
 */
void new_magnet_cb(void);
