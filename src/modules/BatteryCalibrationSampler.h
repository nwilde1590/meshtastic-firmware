#pragma once

#include "configuration.h"

#if HAS_SCREEN

#include "concurrency/OSThread.h"

class BatteryCalibrationSampler : private concurrency::OSThread
{
  public:
    struct BatterySample {
        uint16_t voltageMv;
        uint32_t timestampMs;
    };

    static constexpr uint16_t kMaxSamples = 256;
    static constexpr uint32_t kDefaultDisplayWindowMs = 15 * 60 * 1000;

    BatteryCalibrationSampler();

    void startSampling();
    void stopSampling();
    bool isSampling() const { return active; }
    void resetSamples();
    void setDisplayWindowMs(uint32_t displayWindowMs);
    uint32_t getDisplayWindowMs() const { return displayWindowMs; }
    void getSamples(const BatterySample *&samplesOut, uint16_t &countOut, uint16_t &startOut) const;

  protected:
    int32_t runOnce() override;

  private:
    BatterySample samples[kMaxSamples]{};
    uint16_t sampleCount = 0;
    uint16_t sampleStart = 0;
    uint32_t lastSampleMs = 0;
    uint32_t sampleIntervalMs = 1000;
    uint32_t displayWindowMs = kDefaultDisplayWindowMs;
    bool active = false;

    void appendSample(uint16_t voltageMv, uint32_t nowMs);
};

extern BatteryCalibrationSampler *batteryCalibrationSampler;
#endif