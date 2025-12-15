#pragma once
#include <CTelemetry.h>
class CTeleValue : public CTelemetry {
private:
    constexpr static uint8_t SUBGROUP = 0x00;
    inline static uint32_t instanceCounter{};


    uint32_t _value{};

public:
    CTeleValue(TeleGroup group = TeleGroup::PROGRAM, uint16_t id = 0xFFFF) : CTelemetry(group, SUBGROUP, id) {
        if (id == 0xFFFF) 
            ID = instanceCounter++;
        
        _register(this);
    }


    inline void set(uint32_t value) {
        _value = value;
    }

    float getValue() override {
        uint32_t retVal = _value;
        _value = 0;  // need to reset value after reading
        return retVal;
    }
    
    const char* getName() const override { return "CTeleValue"; }
};