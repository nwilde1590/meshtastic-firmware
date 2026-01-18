#include "BatteryCalibrationSampler.h"
#include "configuration.h"

#if HAS_SCREEN

#include <Arduino.h>
#include "power.h"

BatteryCalibrationSampler *batteryCalibrationSampler;

BatteryCalibrationSampler::BatteryCalibrationSampler() : concurrency::OSThread("BatteryCalibrationSampler")
{
    batteryCalibrationSampler = this;
    setDisplayWindowMs(kDefaultDisplayWindowMs);
    disable();
}

void BatteryCalibrationSampler::startSampling()
{
    active = true;
    enabled = true;
    setIntervalFromNow(0);
}

void BatteryCalibrationSampler::stopSampling()
{
    active = false;
    disable();
}

void BatteryCalibrationSampler::resetSamples()
{
    sampleCount = 0;
    sampleStart = 0;
    lastSampleMs = 0;
}

void BatteryCalibrationSampler::setDisplayWindowMs(uint32_t displayWindowMs)
{
    const uint32_t intervalMs = displayWindowMs / kMaxSamples;
    sampleIntervalMs = (intervalMs > 0) ? intervalMs : 1;
}

void BatteryCalibrationSampler::getSamples(const BatterySample *&samplesOut, uint16_t &countOut, uint16_t &startOut) const
{
    samplesOut = samples;
    countOut = sampleCount;
    startOut = sampleStart;
}

void BatteryCalibrationSampler::appendSample(uint16_t voltageMv, uint32_t nowMs)
{

    lastSampleMs = nowMs;

    uint16_t index = 0;
    if (sampleCount < kMaxSamples) {
        index = static_cast<uint16_t>((sampleStart + sampleCount) % kMaxSamples);
        sampleCount++;
    } else {
        index = sampleStart;
        sampleStart = static_cast<uint16_t>((sampleStart + 1) % kMaxSamples);
    }

    samples[index].voltageMv = voltageMv;
    samples[index].timestampMs = nowMs;
}

int32_t BatteryCalibrationSampler::runOnce()
{
    if (!active) {
        return disable();
    }

    const uint32_t nowMs = millis();
    if (!powerStatus || !powerStatus->getHasBattery()) {
        resetSamples();
        return sampleIntervalMs;
    }

    appendSample(static_cast<uint16_t>(powerStatus->getBatteryVoltageMv()), nowMs);
    return sampleIntervalMs;
}

#endif