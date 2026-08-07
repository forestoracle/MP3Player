#ifndef TDA7419_H
#define TDA7419_H
#include <Wire.h>

class TDA7419 {
  public:
    TDA7419();
    /** 
         @brief Main Source Selector

         @param input Source Selector:  0...5 (QD/SE: QD, SE1, SE2, SE3, QD/SE: SE, mute)
         @param gain Input Gain:  0...15 (0...+15 dB)
         @param auto_zero Auto Zero:  0...1 (on, off)
      */
    void setInput(uint8_t input, uint8_t gain, uint8_t auto_zero);

    /**
         @brief Main Loudness. Attenuation, Center Frequency, High Boost, Loudness Soft Step)
    
         @param attenuation Attenuation: 0...15 (0...-15 dB)
         @param center_frequency Center Frequency: 0...3 (Flat, 400 Hz, 800 Hz, 2400 Hz)
         @param high_boost High Boost: 0...1 (on, off)
         @param softstep Loudness Soft Step:   0...1 (on, off)
      */
    void setMainLoudness(uint8_t attenuation,
                         uint8_t center_frequency,
                         uint8_t high_boost,
                         uint8_t softstep);

    /**
         @brief Soft Mute, Pin Influence for Mute, Soft Mute Time, Soft Step Time, Clock Fast Mode

         @param soft_mute Soft Mute: 0..1 (on/off)
         @param pin Pin Influence for Mute: 0..1 (Pin&IIC, IIC)
         @param soft_mute_time Soft Mute Time: 0..2 (0.48ms, 0.96ms, 123ms)
         @param softstep_time Soft Step Time: 0...7 (0.160ms, 0.321ms, 0.642ms, 1.28ms, 2.26ms, 5.12ms, 10.24ms, 20.48ms)
         @param clock_fast_mode Clock Fast Mode: 0..1 (on/off)
    */
    void setSoftMute(uint8_t soft_mute,
                     uint8_t pin,
                     uint8_t soft_mute_time,
                     uint8_t softstep_time,
                     uint8_t clock_fast_mode);

    /**
          @brief Volume control 

          @param volume Volume
          @param softstep Soft Step: 0..1 (on/off) 
    */
    void setVolume(uint8_t volume, uint8_t softstep);

    /**
         @brief Attenuation control

         @param attenuation Gain/Attenuation: 0...80 (80...0) 0 - mute
         @param softstep Soft Step: 0..1 (on/off)
         */
    void setAttenuationLeftFront(uint8_t attenuation, uint8_t softstep);
    void setAttenuationRightFront(uint8_t attenuation, uint8_t softstep);
    void setAttenuationLeftRear(uint8_t attenuation, uint8_t softstep);
    void setAttenuationRightRear(uint8_t attenuation, uint8_t softstep);
    void setAttenuationSubwoofer(uint8_t attenuation, uint8_t softstep);
    void setAttenuationMixing(uint8_t attenuation, uint8_t softstep);

    /** 
         @brief Treble Filter

         @param gain Gain/Attenuation: 0..31, (-15 dB..15 dB)
         @param center_frequency Treble Center Frequency: 0..3 (10kHz 12.5kHz 15 kHz 17.5 kHz)
         @param ref_out_select Reference Output Select: 0..1 (External Vref = 4V, Internal Vref = 3.3V)
    */
    void setFilter_Treble(uint8_t gain, uint8_t center_frequency, uint8_t ref_out_select);

    /**
         @brief Middle Filter

         @param middle_g Gain/Attenuation: 0..31, (-15 dB..15 dB)
         @param q_factor Middle Q Factor: 0...3 (0.5, 0.75, 1, 1.25)
         @param softstep Middle Soft Step: 0..1 (on/off)
    */
    void setFilter_Middle(uint8_t gain, uint8_t q_factor, uint8_t softstep);

    /**
         @brief Bass Filter
   
         @param gain Gain/Attenuation: 0..31, (-15 dB..15 dB)
         @param q_factor Bass Q Factor: 0..3, (1.0, 1.25, 1.5, 2.0)
         @param softstep Bass Soft Step: 0..1 (on/off)
    */
    void setFilter_Bass(uint8_t gain, uint8_t q_factor, uint8_t softstep);

    /**  
         @brief Second Source Selector 
    
         @param input Source Selector: 0...5 (QD/SE: QD, SE1, SE2, SE3, QD/SE: SE, mute)
         @param gain Input Gain: 0...15 (0...+15 dB)
         @param rear_source Rear speaker source: 0...1 (Main Source/Second Source)
    */
    void setInput2(uint8_t input, uint8_t gain, uint8_t rear_source);

    /**  
         @brief Subwoofer / Middle / Bass 

         @param subwoofer Subwoofer Cut-off Frequency: 0..3 (flat, 80Hz, 120Hz, 160Hz)
         @param middle Middle Center Frequency:  0..3 (500Hz, 1000Hz, 1500Hz, 2500Hz)
         @param bass Bass Center Frequency:  0..3 (60Hz, 80Hz, 100Hz, 200Hz)
         @param bass_dc_mode  Bass DC Mode: 0..1 (on/off)
         @param smoothing_filter Smoothing Filter:  0..1 (on/off (bypass))
   */
    void setSub_M_B(uint8_t subwoofer,
                    uint8_t middle,
                    uint8_t bass,
                    uint8_t bass_dc_mode,
                    uint8_t smoothing_filter);

    /**
          @brief Mixing / gain effect

          @param left_front Mixing to left front speaker: 0..1 (on/off) 
          @param right_front Mixing to right front speaker: 0..1 (on/off)
          @param enable Mixing enable: 0..1 (on/off)
          @param subwoofer_enable Subwoofer enable (OUTLR2 & OUTRR2): 0..1 (on/off)
          @param gain_effect Gain effect for HPF filter: 0..10
     */
    void setMix_Gain_Eff(uint8_t left_front,
                         uint8_t right_front,
                         uint8_t enable,
                         uint8_t subwoofer_enable,
                         uint8_t gain_effect);

    /**
         @brief Spectrum Analyser setup.

         @param q_factor Filter Q Factor: 0..1 (3.5, 1.75)
         @param reset_mode Reset mode: 0..1 (IIC, Auto)
         @param source Source: 0..1 (Bass, In gain)
         @param run Run: 0..1 (on/off)
         @param reset Reset: 0..1 (on/off)
         @param clock_source Clock source: 0..1 (internal, external)
         @param coupling_mode Coupling mode: 0..3 (DC Coupling (without HPF), AC coupling after In gain, DC Coupling (with HPF), AC coupling after Bass)
   */
    void setSpectrumAnalyzer(uint8_t q_factor,
                             uint8_t reset_mode,
                             uint8_t source,
                             uint8_t run,
                             uint8_t reset,
                             uint8_t clock_source,
                             uint8_t coupling_mode);

  private:
    void writeWire(uint8_t subaddress, uint8_t data);
    void setAttenuation(uint8_t subaddress, uint8_t attenuation, bool softstep);
};

#endif  //TDA7419_H
