#pragma once

#include "SinglePortModule.h"

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
};
extern BatteryCalibrationModule *batteryCalibrationModule;