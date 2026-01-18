#pragma once

#include "SinglePortModule.h"
#include "BatteryCalibrationSampler.h"

class BatteryCalibrationModule : public SinglePortModule
{
  public:
    BatteryCalibrationModule();

#if HAS_SCREEN
    bool wantUIFrame() override { return true; }
    void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;
#endif

  protected:
    ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;

#if HAS_SCREEN
  private:
    void computeGraphBounds(OLEDDisplay *display, int16_t x, int16_t y, int16_t &graphX, int16_t &graphY, int16_t &graphW,
                            int16_t &graphH);
    void drawBatteryGraph(OLEDDisplay *display, int16_t graphX, int16_t graphY, int16_t graphW, int16_t graphH,
                          const BatteryCalibrationSampler::BatterySample *samples, uint16_t sampleCount, uint16_t sampleStart);
#endif

};
extern BatteryCalibrationModule *batteryCalibrationModule;