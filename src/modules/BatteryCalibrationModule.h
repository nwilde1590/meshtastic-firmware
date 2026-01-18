#pragma once

#include "SinglePortModule.h"

class BatteryCalibrationModule : public SinglePortModule
{
  public:
    BatteryCalibrationModule();

#if HAS_SCREEN
    bool wantUIFrame() override { return true; }
    void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;
    void resetGraphSamples();
#endif

  protected:
    ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;

#if HAS_SCREEN
  private:
    struct BatterySample {
        uint16_t voltageMv;
        uint32_t timestampMs;
    };

    static constexpr uint32_t kSampleIntervalMs = 1000;
    static constexpr uint16_t kMaxSamples = 256;

    BatterySample samples[kMaxSamples]{};
    uint16_t sampleCount = 0;
    uint16_t sampleStart = 0;
    uint32_t lastSampleMs = 0;

    void resetSamples();
    void appendSample(uint16_t voltageMv, uint32_t nowMs);
    void computeGraphBounds(OLEDDisplay *display, int16_t x, int16_t y, int16_t &graphX, int16_t &graphY, int16_t &graphW,
                            int16_t &graphH);
    void drawBatteryGraph(OLEDDisplay *display, int16_t graphX, int16_t graphY, int16_t graphW, int16_t graphH);
#endif

};
extern BatteryCalibrationModule *batteryCalibrationModule;