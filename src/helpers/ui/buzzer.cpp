#include "Arduino.h"
#ifdef PIN_BUZZER
#include "buzzer.h"

void genericBuzzer::begin() {
//    Serial.print("DBG: Setting up buzzer on pin ");
//    Serial.println(PIN_BUZZER);
    #ifdef PIN_BUZZER_EN
      pinMode(PIN_BUZZER_EN, OUTPUT);
      digitalWrite(PIN_BUZZER_EN, HIGH);
    #endif

    quiet(false);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW); // need to pull low by default to avoid extreme power draw
}

void genericBuzzer::play(const char *melody, bool force) {
    if (isPlaying())   // interrupt existing
    {
        rtttl::stop();
    }

    if (_is_quiet && !force) return;
#ifdef PIN_BUZZER_EN
    if (_is_quiet) digitalWrite(PIN_BUZZER_EN, HIGH);  // forced play while quiet; quiet(true) restores
#endif

    rtttl::begin(PIN_BUZZER,melody);
//    Serial.print("DBG: Playing melody - isQuiet: ");
//    Serial.println(isQuiet());
}

// same LEDC usage as NonBlockingRTTTL on ESP32 (one pin, channel 0), Arduino tone() elsewhere
void genericBuzzer::toneOn(uint16_t hz, bool force) {
    if (isPlaying()) rtttl::stop();
    if (_is_quiet && !force) return;
#ifdef PIN_BUZZER_EN
    if (_is_quiet) digitalWrite(PIN_BUZZER_EN, HIGH);  // forced tone while quiet; quiet(true) restores
#endif
#if defined(ESP32)
  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    static bool attached = false;
    if (!attached) attached = ledcAttach(PIN_BUZZER, 1000, 10);
    ledcWriteTone(PIN_BUZZER, hz);
  #else
    ledcAttachPin(PIN_BUZZER, 0);
    ledcWriteTone(0, hz);
  #endif
#else
    tone(PIN_BUZZER, hz);
#endif
    _tone_on = true;
}

void genericBuzzer::toneOff() {
    if (!_tone_on) return;
#if defined(ESP32)
  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ledcWriteTone(PIN_BUZZER, 0);
  #else
    ledcWriteTone(0, 0);
  #endif
#else
    noTone(PIN_BUZZER);
#endif
    _tone_on = false;
}

bool genericBuzzer::isPlaying() {
    return rtttl::isPlaying();
}

void genericBuzzer::loop() {
    if (!rtttl::done()) rtttl::play();
}

void genericBuzzer::startup() {
    play(startup_song);
}

void genericBuzzer::shutdown() {
    play(shutdown_song);
}

void genericBuzzer::quiet(bool buzzer_state) {
    _is_quiet = buzzer_state;
#ifdef PIN_BUZZER_EN
    if (_is_quiet) {
      digitalWrite(PIN_BUZZER_EN, LOW);
    } else {
      digitalWrite(PIN_BUZZER_EN, HIGH);
    }
#endif
}

bool genericBuzzer::isQuiet() {
    return _is_quiet;
}

#endif  // ifdef PIN_BUZZER