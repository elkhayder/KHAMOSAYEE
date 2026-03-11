#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 6

// Servo : 50Hz = période 20ms
// Sur Pico avec clock 125MHz et diviser par 64 -> 1953125 Hz / 64 = ~30517 Hz
// wrap = 39062 pour avoir 50Hz... on utilise plutôt :
// clkdiv = 64, wrap = 39062 -> 125000000 / 64 / 39062 ≈ 50Hz

#define PWM_CLKDIV  64.0f
#define PWM_WRAP    39062

// Conversion ms -> valeur PWM
// 1ms = 1953, 1.5ms = 2929, 2ms = 3906
#define SERVO_MIN   1953   // 0°
#define SERVO_MID   2929   // 90° (centre)
#define SERVO_MAX   3906   // 180°

void servo_init() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(SERVO_PIN);
    
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, PWM_CLKDIV);
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_init(slice, &config, true);
}

void servo_set_angle(uint angle) {
    if (angle > 180) angle = 180;
    uint16_t val = SERVO_MIN + (angle * (SERVO_MAX - SERVO_MIN)) / 180;
    pwm_set_gpio_level(SERVO_PIN, val);
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);
    
    printf("=== Test Servo Direction GP6 ===\n");
    servo_init();

    while (true) {
        printf("Position 0° (gauche)\n");
        servo_set_angle(0);
        sleep_ms(1000);

        printf("Position 90° (centre)\n");
        servo_set_angle(90);
        sleep_ms(1000);

        printf("Position 180° (droite)\n");
        servo_set_angle(180);
        sleep_ms(1000);
    }

    return 0;
}