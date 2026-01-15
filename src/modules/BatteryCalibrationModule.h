#pragma once

#include "configuration.h"

#if HAS_SCREEN

#include "Observer.h"
#include "input/InputBroker.h"
#include "mesh/MeshModule.h"

class BatteryCalibrationModule : public MeshModule
{
  public:
    BatteryCalibrationModule();

    virtual bool wantUIFrame() override { return true; }
    virtual void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;

  protected:
    virtual bool wantPacket(const meshtastic_MeshPacket *p) override { return false; }

  private:
    int handleInputEvent(const InputEvent *event);
    CallbackObserver<BatteryCalibrationModule, const InputEvent *> inputObserver =
        CallbackObserver<BatteryCalibrationModule, const InputEvent *>(this, &BatteryCalibrationModule::handleInputEvent);
};

#endif
