#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// --- Configuration CMPS12 ---
#define CMPS12_ADDR     0x60
#define REG_FIRST_RAW   0x06
#define REG_NAV_DATA    0x02
#define REG_CALIB_STAT  0x1E

// --- Pins I2C ---
#define I2C_PORT  i2c1
#define PIN_SDA   26
#define PIN_SCL   27

// --- Init I2C ---
void cmps12_init() {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
}

// --- Lecture données brutes ---
void read_cmps12_data() {
    uint8_t reg = REG_FIRST_RAW;
    uint8_t buffer[18];

    if (i2c_write_blocking(I2C_PORT, CMPS12_ADDR, &reg, 1, true) != 1) {
        printf("[ERREUR] Écriture I2C échouée\n");
        return;
    }
    if (i2c_read_blocking(I2C_PORT, CMPS12_ADDR, buffer, 18, false) != 18) {
        printf("[ERREUR] Lecture I2C échouée\n");
        return;
    }

    int16_t mag_x  = (buffer[0]  << 8) | buffer[1];
    int16_t mag_y  = (buffer[2]  << 8) | buffer[3];
    int16_t mag_z  = (buffer[4]  << 8) | buffer[5];
    int16_t acc_x  = (buffer[6]  << 8) | buffer[7];
    int16_t acc_y  = (buffer[8]  << 8) | buffer[9];
    int16_t acc_z  = (buffer[10] << 8) | buffer[11];
    int16_t gyro_x = (buffer[12] << 8) | buffer[13];
    int16_t gyro_y = (buffer[14] << 8) | buffer[15];
    int16_t gyro_z = (buffer[16] << 8) | buffer[17];

    printf("MAG=[%6d %6d %6d] ACC=[%6d %6d %6d] GYRO=[%6d %6d %6d]\n",
           mag_x, mag_y, mag_z, acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z);
}

// --- Lecture navigation ---
void read_navigation_data() {
    uint8_t reg = REG_NAV_DATA;
    uint8_t buffer[4];

    i2c_write_blocking(I2C_PORT, CMPS12_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, CMPS12_ADDR, buffer, 4, false);

    uint16_t angle16 = (buffer[0] << 8) | buffer[1];
    float cap = angle16 / 10.0f;
    int8_t pitch = (int8_t)buffer[2];
    int8_t roll  = (int8_t)buffer[3];

    printf("Cap: %5.1f deg | Pitch: %3d deg | Roll: %3d deg\n", cap, pitch, roll);
}

// --- Calibration ---
void check_calibration() {
    uint8_t reg = REG_CALIB_STAT;
    uint8_t status;

    i2c_write_blocking(I2C_PORT, CMPS12_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, CMPS12_ADDR, &status, 1, false);

    int mag_cal  =  status        & 0x03;
    int acc_cal  = (status >> 2)  & 0x03;
    int gyro_cal = (status >> 4)  & 0x03;
    int sys_cal  = (status >> 6)  & 0x03;

    printf("Calib -> Mag:%d Acc:%d Gyro:%d Sys:%d\n", mag_cal, acc_cal, gyro_cal, sys_cal);
}

// --- Main ---
int main() {
    stdio_init_all();

    // Attendre connexion USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("\n=== Char a Voile - CMPS12 ===\n");
    cmps12_init();

    while (true) {
        check_calibration();
        read_cmps12_data();
        read_navigation_data();
        printf("---\n");
        sleep_ms(500);
    }

    return 0;
}