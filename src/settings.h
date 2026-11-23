#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

// Firmware version - increment when Settings structure changes
#define FIRMWARE_VERSION 3

// Semantic version string (replaced by GitHub Action during release builds)
#ifndef FIRMWARE_VERSION_STRING
#define FIRMWARE_VERSION_STRING "dev"
#endif

struct Settings {
    uint16_t version;          // Firmware version for compatibility check
    int brightness;
    int theme;
    char lastImage[64];
    long gmtOffset;            // GMT offset in seconds
    bool valid;
    uint32_t crc;              // CRC32 checksum for data integrity

    // Automatic brightness settings
    bool autoBrightness;
    float luxOff;
    float luxOn;
    float luxMax;
    uint8_t brightnessMin;
    uint8_t brightnessMax;
};

// Boot failure tracking structure (separate from settings)
struct BootCounter {
    uint16_t magic;            // Magic number to validate boot counter
    uint8_t failCount;         // Number of consecutive boot failures
    uint32_t lastBootTime;     // Timestamp of last successful boot
};

// Power cycle reset structure (user-initiated factory reset)
struct PowerCycleCounter {
    uint16_t magic;            // Magic number to validate power cycle counter (0x5C01)
    uint8_t cycleCount;        // Number of quick power cycles
};

void settingsInit();
void settingsLoad(Settings &settings);
void settingsSave(const Settings &settings);
void settingsReset(Settings &settings) {
    Serial.println(F("Resetting settings to factory defaults"));

    memset(&settings, 0, sizeof(Settings));

    settings.version = FIRMWARE_VERSION;
    settings.brightness = 70;
    settings.theme = 0;
    settings.lastImage[0] = '\0';
    settings.gmtOffset = 3600;
    settings.valid = true;

    // Automatic brightness defaults
    settings.autoBrightness = true;
    settings.luxOff = 2.0f;
    settings.luxOn = 5.0f;
    settings.luxMax = 500.0f;
    settings.brightnessMin = 5;
    settings.brightnessMax = 80;

    settings.crc = settingsCalculateCRC(settings);
}
uint32_t settingsCalculateCRC(const Settings &settings);
bool settingsValidate(const Settings &settings);

// Boot counter functions
void bootCounterInit();
uint8_t bootCounterGet();
void bootCounterIncrement();
void bootCounterReset();
bool bootCounterCheckFailsafe();

// Power cycle counter functions (user-initiated factory reset)
void powerCycleCounterInit();
uint8_t powerCycleCounterGet();
void powerCycleCounterIncrement();
void powerCycleCounterReset();
bool powerCycleCounterCheckReset();

#endif
