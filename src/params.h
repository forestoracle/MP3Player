struct Params {
    // DAC
    float DACGaint;  // Усиление DAC. От 0 до 2. Больше не надо наверное.
    // TDA7419
    uint8_t input;   // Вход
    uint8_t gain;    // Усиление
    bool auto_zero;  // Auto-zero
};