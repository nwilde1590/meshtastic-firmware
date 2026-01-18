#include "BatteryCalibrationModule.h"
#include "graphics/SharedUIDisplay.h"
#include "graphics/ScreenFonts.h"
#include "power.h"

BatteryCalibrationModule *batteryCalibrationModule;

BatteryCalibrationModule::BatteryCalibrationModule()
    : SinglePortModule("battery-calibration", meshtastic_PortNum_PRIVATE_APP)
{
    batteryCalibrationModule = this;
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

    char voltageStr[12] = {0};
    char percentStr[8] = {0};
    if (powerStatus && powerStatus->getHasBattery()) {
        const int batV = powerStatus->getBatteryVoltageMv() / 1000;
        const int batCv = (powerStatus->getBatteryVoltageMv() % 1000) / 10;
        snprintf(voltageStr, sizeof(voltageStr), "%01d.%02dV", batV, batCv);
        snprintf(percentStr, sizeof(percentStr), "%3d%%", powerStatus->getBatteryChargePercent());
    } else {
        snprintf(voltageStr, sizeof(voltageStr), "USB");
        snprintf(percentStr, sizeof(percentStr), "USB");
    }

    const int lineY = graphics::getTextPositions(display)[1];
    display->drawString(x, lineY, voltageStr);
    display->drawString(x + SCREEN_WIDTH - display->getStringWidth(percentStr), lineY, percentStr);

}
#endif