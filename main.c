#include <stdio.h>
#include "pico/stdlib.h"
#include "cmps12.h"
#include "servo_direction.h"
#include "pid.h"

#define DT                  0.15f   // 150ms
#define DEADBAND            2.0f    // zone morte en degrés
#define SERVO_CENTER_OFFSET 133.0f  // offset mécanique calibré

static float heading_error_to_north(float heading_deg) {
    if (heading_deg > 180.0f) heading_deg -= 360.0f;
    return heading_deg;
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);

    cmps12_init();
    servo_init();

    const int steering_sign = 1;

    PID pid_cap;
    pid_init(&pid_cap, 0.25f, 0.0f, 0.05f, 30.0f);

    printf("=== North hold (PID) ===\n");
    printf("Servo center offset: %.1f deg\n", SERVO_CENTER_OFFSET);

    while (true) {
        NavData nav;
        CalibData calib;

        if (!read_navigation_data(&nav)) {
            servo_set_angle(SERVO_CENTER_OFFSET);
            pid_reset(&pid_cap);
            printf("Compass read failed, steering center\n");
            sleep_ms((uint32_t)(DT * 1000));
            continue;
        }

        bool calib_ok = check_calibration(&calib);

        float err = heading_error_to_north(nav.cap);

        if (err > -DEADBAND && err < DEADBAND) {
            servo_set_angle(SERVO_CENTER_OFFSET);
            pid_reset(&pid_cap);
            printf("Cap:%.1f Pitch:%d Roll:%d | Err:%.1f | DEADBAND | Servo:%.1f",
                   nav.cap, nav.pitch, nav.roll, err, SERVO_CENTER_OFFSET);
        } else {
            float correction = steering_sign * pid_compute(&pid_cap, err, DT);
            float angle = SERVO_CENTER_OFFSET + correction;
            servo_set_angle(angle);
            printf("Cap:%.1f Pitch:%d Roll:%d | Err:%+.1f | Corr:%+.1f | Servo:%.1f",
                   nav.cap, nav.pitch, nav.roll, err, correction, angle);
        }

        if (calib_ok) {
            printf(" | Calib M:%d A:%d G:%d S:%d\n",
                   calib.mag_cal, calib.acc_cal, calib.gyro_cal, calib.sys_cal);
        } else {
            printf(" | Calib read failed\n");
        }

        sleep_ms((uint32_t)(DT * 1000));
    }

    return 0;
}