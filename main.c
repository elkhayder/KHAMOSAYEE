#include <stdio.h>
#include "pico/stdlib.h"

#include "cmps12.h"
#include "servo_direction.h"

static float heading_to_servo_angle(float heading_deg) {
    // Map compass heading [0..360] to servo angle [0..180]
    if (heading_deg < 0.0f) {
        heading_deg = 0.0f;
    }
    if (heading_deg > 360.0f) {
        heading_deg = 360.0f;
    }
    return heading_deg * 0.5f;
}

int main(void) {
    stdio_init_all();
    cmps12_init();
    servo_init();
    servo_set_angle(90.0f);

    while (true) {
        NavData nav;
        CalibData calib;

        bool nav_ok = read_navigation_data(&nav);
        bool calib_ok = check_calibration(&calib);

        if (nav_ok) {
            float servo_angle = heading_to_servo_angle(nav.cap);
            servo_set_angle(servo_angle);
            printf("Cap: %.1f deg, Servo: %.1f deg\n", nav.cap, servo_angle);
        } else {
            servo_set_angle(90.0f);
            printf("Navigation read failed\n");
        }

        if (calib_ok) {
            printf("Calib M:%d A:%d G:%d S:%d\n",
                   calib.mag_cal,
                   calib.acc_cal,
                   calib.gyro_cal,
                   calib.sys_cal);
        }

        sleep_ms(200);
    }
}
