#include "BatteryCalibrationModule.h"
#include "graphics/SharedUIDisplay.h"
#include "graphics/ScreenFonts.h"

BatteryCalibrationModule::BatteryCalibrationModule()
    : SinglePortModule("battery-calibration", meshtastic_PortNum_PRIVATE_APP)
{
}

ProcessMessage BatteryCalibrationModule::handleReceived(const meshtastic_MeshPacket &mp)
{
    (void)mp;
    return ProcessMessage::CONTINUE;
}

#if HAS_SCREEN
void BatteryCalibrationModule::drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    (void)state;
    display->clear();
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(FONT_SMALL);

    const char *titleStr = "Battery Calibration";

    graphics::drawCommonHeader(display, x, y, titleStr);
}
#endif