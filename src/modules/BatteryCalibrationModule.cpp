#include "configuration.h"

#if HAS_SCREEN

#include "BatteryCalibrationModule.h"
#include "graphics/ScreenFonts.h"
#include "graphics/SharedUIDisplay.h"
#include "main.h"
#include "mesh/generated/meshtastic/module_config.pb.h"

BatteryCalibrationModule::BatteryCalibrationModule() : MeshModule("BatteryCalibration")
{
    if (inputBroker) {
        inputObserver.observe(inputBroker);
    }
}

int BatteryCalibrationModule::handleInputEvent(const InputEvent *event)
{
    if (event->inputEvent == static_cast<char>(meshtastic_ModuleConfig_CannedMessageConfig_InputEventChar_BACK) ||
        event->inputEvent == static_cast<char>(meshtastic_ModuleConfig_CannedMessageConfig_InputEventChar_CANCEL)) {
        if (screen) {
            screen->showPrevFrame();
        }
    }
    return 0;
}

void BatteryCalibrationModule::drawFrame(OLEDDisplay *display, OLEDDisplayUiState *, int16_t x, int16_t y)
{
    if (!screen)
        return;
    display->clear();
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(FONT_SMALL);
    int line = 1;

    // === Set Title
    const char *titleStr = "Battery Calibration";

    // === Header ===
    graphics::drawCommonHeader(display, x, y, titleStr);

    if (powerStatus->getHasBattery()) {
        char voltageStr[24];
        const int batV = powerStatus->getBatteryVoltageMv() / 1000;
        const int batCv = (powerStatus->getBatteryVoltageMv() % 1000) / 10;
        snprintf(voltageStr, sizeof(voltageStr), "Voltage: %01d.%02dV", batV, batCv);
        display->drawString(x, graphics::getTextPositions(display)[line++], voltageStr);
    } else {
        display->drawString(x, graphics::getTextPositions(display)[line++], "Voltage: USB");
    }
}

#endif
