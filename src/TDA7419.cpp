#include "TDA7419.h"

const uint8_t TDA7419_ADDRESS = 0x44;  
// Субадреса
const uint8_t TDA7419_INPUT = 0b01000000;      /// Main Source Selector
const uint8_t TDA7419_LOUDNESS = 0b01000001;   /// Main Loudness
const uint8_t TDA7419_SOFT = 0b01000010;       /// Soft Mute / Clock Generator
const uint8_t TDA7419_VOLUME = 0b01000011;     /// Volume
const uint8_t TDA7419_TREBLE = 0b01000100;     /// Treble
const uint8_t TDA7419_MIDDLE = 0b01000101;     /// Middle
const uint8_t TDA7419_BASS = 0b01000110;       /// Bass
const uint8_t TDA7419_INPUT2 = 0b01000111;     /// Second Source Selector
const uint8_t TDA7419_SUB_M_B = 0b01001000;    /// Subwoofer / Middle / Bass
const uint8_t TDA7419_MIX_G_EFF = 0b01001001;  /// Mixing / Gain / Effect
const uint8_t TDA7419_ATT_LF = 0b01001010;     /// Speaker Attenuator Left Front
const uint8_t TDA7419_ATT_RF = 0b01001011;     /// Speaker Attenuator Right Front
const uint8_t TDA7419_ATT_LT = 0b01001100;     /// Speaker Attenuator Left Rear
const uint8_t TDA7419_ATT_RT = 0b01001101;     /// Speaker Attenuator Right Rear
const uint8_t TDA7419_MIX_LEV = 0b01001110;    /// Mixing Level Control
const uint8_t TDA7419_ATT_SUB = 0b01001111;    /// Subwoofer Attenuator
const uint8_t TDA7419_SPECTRUM = 0b01010000;   /// Spectrum Analyzer / Clock Source / AC Mode

TDA7419::TDA7419() {
    // Конструктор пустой
}

void TDA7419::setInput(uint8_t input, uint8_t gain, bool auto_zero) {
    writeWire(TDA7419_INPUT, input | (gain << 3) | (auto_zero ? 0b10000000 : 0b00000000));
}

void TDA7419::setMainLoudness(uint8_t attenuation, uint8_t center_frequency, bool high_boost, bool softstep) {
    writeWire(TDA7419_LOUDNESS,
              attenuation | (center_frequency << 4) | (high_boost ? 0b01000000 : 0b00000000) | (softstep ? 0b10000000 : 0b00000000));
}

void TDA7419::setSoftMute(uint8_t soft_mute, uint8_t pin, uint8_t soft_mute_time, uint8_t softstep_time, uint8_t clock_fast_mode) {
    writeWire(TDA7419_SOFT, soft_mute
                                | pin << 1
                                | (soft_mute_time << 2)
                                | (softstep_time << 4)
                                | (clock_fast_mode << 7));
}

void TDA7419::setVolume(uint8_t volume, uint8_t softstep) {
    if (volume < 0) { volume = abs(volume) + 16; }  //-79 ... 0 dB
    writeWire(TDA7419_VOLUME, volume | (softstep > 1 ? 1 : 0) << 7);
}

void TDA7419::setAttenuation(uint8_t subaddress, uint8_t attenuation, bool softstep) {
    attenuation = 95 - attenuation;
    writeWire(subaddress, attenuation | (softstep > 1 ? 1 : softstep) << 7);
}

void TDA7419::setAttenuation_LF(uint8_t attenuation, uint8_t softstep) {
    setAttenuation(TDA7419_ATT_LF, attenuation, softstep);
}

void TDA7419::setAttenuation_RF(uint8_t attenuation, uint8_t softstep) {
    setAttenuation(TDA7419_ATT_RF, attenuation, softstep);
}

void TDA7419::setAttenuation_LT(uint8_t attenuation, uint8_t softstep) {
    setAttenuation(TDA7419_ATT_LT, attenuation, softstep);
}

void TDA7419::setAttenuation_RT(uint8_t attenuation, uint8_t softstep) {
    setAttenuation(TDA7419_ATT_RT, attenuation, softstep);
}

void TDA7419::setAttenuation_Subwoofer(uint8_t attenuation, uint8_t softstep) {
    setAttenuation(TDA7419_ATT_SUB, attenuation, softstep);
}

void TDA7419::setAttenuation_Mixing(uint8_t attenuation, uint8_t softstep) {
    setAttenuation(TDA7419_MIX_LEV, attenuation, softstep);
}


void TDA7419::setFilter_Treble(uint8_t gain, uint8_t center_frequency, uint8_t ref_out_select) {
    writeWire(TDA7419_TREBLE, (gain > 31 ? 31 : gain)
                                  | (center_frequency > 3 ? 3 : center_frequency) << 5
                                  | (ref_out_select > 1 ? 1 : ref_out_select) << 7);
}


void TDA7419::setFilter_Middle(uint8_t gain, uint8_t q_factor, uint8_t softstep) {
    writeWire(TDA7419_MIDDLE, (gain > 31 ? 31 : gain)
                                  | (q_factor > 3 ? 3 : q_factor) << 5
                                  | (softstep > 1 ? 1 : softstep) << 7);
}

void TDA7419::setFilter_Bass(uint8_t gain, uint8_t q_factor, uint8_t softstep) {
    writeWire(TDA7419_BASS, (gain > 31 ? 31 : gain)
                                | (q_factor > 3 ? 3 : q_factor) << 5
                                | (softstep > 1 ? 1 : softstep) << 7);
}

void TDA7419::setInput2(uint8_t input, uint8_t gain, uint8_t rear_source) {
    writeWire(TDA7419_INPUT2, input > 5 ? 5 : input | (gain > 15 ? 15 : gain) << 3 | (rear_source > 1 ? 1 : rear_source) << 7);
}

void TDA7419::setSub_M_B(uint8_t subwoofer, uint8_t middle, uint8_t bass, uint8_t bass_dc_mode, uint8_t smoothing_filter) {
    writeWire(TDA7419_SUB_M_B, (subwoofer > 3 ? 3 : subwoofer)
                                   | (middle > 3 ? 3 : middle) << 2
                                   | (bass > 3 ? 3 : bass) << 4
                                   | (bass_dc_mode > 1 ? 1 : bass_dc_mode) << 6
                                   | (smoothing_filter > 1 ? 1 : smoothing_filter) << 7);
}

void TDA7419::setMix_Gain_Eff(uint8_t left_front, uint8_t right_front, uint8_t enable, uint8_t subwoofer_enable, uint8_t gain_effect) {
    writeWire(TDA7419_MIX_G_EFF, (left_front > 1 ? 1 : left_front)
                                     | (right_front > 1 ? 1 : right_front) << 1
                                     | (enable > 1 ? 1 : enable) << 2
                                     | (subwoofer_enable > 1 ? 1 : subwoofer_enable) << 3
                                     | (gain_effect > 10 ? 10 : gain_effect) << 4);
}

void TDA7419::setSpectrumAnalyzer(uint8_t q_factor,
                                  uint8_t reset_mode,
                                  uint8_t source,
                                  uint8_t run,
                                  uint8_t reset,
                                  uint8_t clock_source,
                                  uint8_t coupling_mode) {
    writeWire(TDA7419_SPECTRUM, (q_factor > 1 ? 1 : q_factor)
                                    | (reset_mode > 1 ? 1 : reset_mode) << 1
                                    | (source > 1 ? 1 : source) << 2
                                    | (run > 1 ? 1 : run) << 3
                                    | (reset > 1 ? 1 : reset) << 4
                                    | (clock_source > 1 ? 1 : clock_source) << 5
                                    | (coupling_mode > 3 ? 3 : coupling_mode) << 6);
}

void TDA7419::writeWire(uint8_t subaddress, uint8_t data) {
    Wire.beginTransmission(TDA7419_ADDRESS);
    Wire.write(subaddress);
    Wire.write(data);
    Wire.endTransmission();
}
