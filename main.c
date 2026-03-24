#include <stdio.h>
#include "pico/stdlib.h"
#include "cmps12.h"
#include "servo_direction.h"
#include "servo_voile.h"
#include "pid.h"

#define DT                  0.15f
#define DEADBAND            2.0f
#define SERVO_CENTER_OFFSET 133.0f

// --- Variables globales ---
static PID pid_cap;
static const int steering_sign = 1;
static float voile_angle = VOILE_FERMEE;

// =====================================================
// FONCTIONS
// =====================================================

static float heading_error_to_north(float heading_deg) {
    if (heading_deg > 180.0f) heading_deg -= 360.0f;
    return heading_deg;
}

static void system_init() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);
    cmps12_init();
    servo_init();
    servo_voile_init();
    pid_init(&pid_cap, 0.3f, 0.0f, 0.05f, 30.0f);
    printf("=== Char a Voile - North hold + Voile ===\n");
    printf("Servo direction offset: %.1f deg\n", SERVO_CENTER_OFFSET);
    printf("Commandes voile : 'o'=ouverte 'm'=mi-ouverte 'f'=fermee '+/-'=+/-5deg\n");
}

static void print_calib() {
    CalibData calib;
    if (check_calibration(&calib)) {
        printf(" | Calib M:%d A:%d G:%d S:%d\n",
               calib.mag_cal, calib.acc_cal, calib.gyro_cal, calib.sys_cal);
    } else {
        printf(" | Calib read failed\n");
    }
}

static void steer_to_north() {
    NavData nav;

    if (!read_navigation_data(&nav)) {
        servo_set_angle(SERVO_CENTER_OFFSET);
        pid_reset(&pid_cap);
        printf("Compass read failed, steering center\n");
        return;
    }

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

    print_calib();
}

static void handle_voile_command() {
    int c = getchar_timeout_us(0);  // non bloquant

    if      (c == 'o') voile_angle = VOILE_OUVERTE;
    else if (c == 'm') voile_angle = VOILE_MI_OUVERTE;
    else if (c == 'f') voile_angle = VOILE_FERMEE;
    else if (c == '+') voile_angle += 5.0f;
    else if (c == '-') voile_angle -= 5.0f;
    else return;  // pas de commande → on ne fait rien

    if (voile_angle > 180.0f) voile_angle = 180.0f;
    if (voile_angle < 0.0f)   voile_angle = 0.0f;

    servo_voile_set_angle(voile_angle);
    printf(">>> Voile : %.1f deg\n", voile_angle);
}

// =====================================================
// MAIN
// =====================================================

int main() {
    system_init();

    while (true) {
        handle_voile_command();  // commande manuelle voile (non bloquant)
        steer_to_north();        // PID direction
        sleep_ms((uint32_t)(DT * 1000));
    }

    return 0;
}