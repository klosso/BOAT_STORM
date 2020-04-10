/*
PPMReader library based on https://github.com/Hasi123/read-any-ppm
Works with Servo library

Paweł (DzikuVx) Spychalski 2017
https://quadmeup.com

License: GNU GPL v3
*/

#ifndef PPMReader_h
#define PPMReader_h

#include <stdint.h>
#define PPMREADER_PMM_CHANNEL_COUNT 16

class PPMReader
{
  public:
    PPMReader(int pin, bool useTimer);
    uint16_t get(const uint8_t channel);
    void start(void);
    void stop(void);
    bool isReceiving(void);

  private:
    static void handler();
    volatile static uint16_t ppm[PPMREADER_PMM_CHANNEL_COUNT];
    int _pin;
};

#endif
