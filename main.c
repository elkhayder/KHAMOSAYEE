#include <stdio.h>
#include "pico/stdlib.h"
#include "cmps12.h"
#include "servo_direction.h"

static float heading_error_to_north(float heading_deg) {
    // Compass heading is [0..360). Target north is 0 deg.
    // Return shortest signed error in [-180..180].
    if (heading_deg > 180.0f) {
        heading_deg -= 360.0f;
    }
    return heading_deg;
}

int main() {
    stdio_init_all();
    cmps12_init();
    servo_init();

    const float servo_center = 90.0f;
    const float servo_left = 60.0f;
    const float servo_right = 120.0f;
    const float deadband_deg = 5.0f;

    // If steering is inverted on your mechanics, set this to -1.
    const int steering_sign = 1;

    printf("=== North hold (no PID) ===\n");

    while (true) {
        NavData nav;
        if (!read_navigation_data(&nav)) {
            servo_set_angle(servo_center);
            printf("Compass read failed, steering center\n");
            sleep_ms(200);
            continue;
        }

        float err = heading_error_to_north(nav.cap);
        float cmd = servo_center;

        if (err > deadband_deg) {
            cmd = (steering_sign > 0) ? servo_left : servo_right;
        } else if (err < -deadband_deg) {
            cmd = (steering_sign > 0) ? servo_right : servo_left;
        }

        servo_set_angle(cmd);
        printf("Heading: %.1f deg | Error: %.1f deg | Servo: %.1f deg\n", nav.cap, err, cmd);
        sleep_ms(150);
    }

    return 0;
}