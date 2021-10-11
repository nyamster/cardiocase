#ifndef TPS_H
#define TPS_H

#include "main.h"

class Tps65070 {
    private:
        I2C_HandleTypeDef i2c;
    public:
        Tps65070(I2C_HandleTypeDef _i2c) : i2c(_i2c) {}
        Tps65070() {}
        void init();
        int voltage_measurment();
};

#endif