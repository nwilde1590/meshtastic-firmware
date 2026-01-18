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
void BatteryCalibrationModule::computeGraphBounds(OLEDDisplay *display, int16_t x, int16_t y, int16_t &graphX, int16_t &graphY,
                                                  int16_t &graphW, int16_t &graphH)
{
    (void)y;
    const int *textPositions = graphics::getTextPositions(display);
    const int16_t lineY = textPositions[1];
    graphX = x;
    graphY = static_cast<int16_t>(lineY + FONT_HEIGHT_SMALL + 2);
    graphW = SCREEN_WIDTH;
    graphH = static_cast<int16_t>(SCREEN_HEIGHT - graphY);
    if (graphH < 0) {
        graphH = 0;
    }
}

void BatteryCalibrationModule::drawBatteryGraph(OLEDDisplay *display, int16_t graphX, int16_t graphY, int16_t graphW, int16_t graphH,
                                                const BatteryCalibrationSampler::BatterySample *samples, uint16_t sampleCount,
                                                uint16_t sampleStart)
{
    if (!samples || sampleCount < 2 || graphW <= 1 || graphH <= 1) {
        return;
    }

    uint16_t minMv = samples[sampleStart].voltageMv;
    uint16_t maxMv = samples[sampleStart].voltageMv;
    for (uint16_t i = 1; i < sampleCount; ++i) {
        const uint16_t sampleIndex = static_cast<uint16_t>((sampleStart + i) % BatteryCalibrationSampler::kMaxSamples);
        const uint16_t voltageMv = samples[sampleIndex].voltageMv;
        if (voltageMv < minMv) {
            minMv = voltageMv;
        }
        if (voltageMv > maxMv) {
            maxMv = voltageMv;
        }
    }

    const uint16_t rangeMv = static_cast<uint16_t>(maxMv - minMv);
    const int32_t xSpan = graphW - 1;
    const int32_t ySpan = graphH - 1;
    const uint16_t maxIndex = static_cast<uint16_t>(sampleCount - 1);

    auto clampY = [&](int16_t yValue) -> int16_t {
        if (yValue < graphY) {
            return graphY;
        }
        const int16_t maxY = static_cast<int16_t>(graphY + ySpan);
        if (yValue > maxY) {
            return maxY;
        }
        return yValue;
    };

    auto voltageToY = [&](uint16_t voltageMv) -> int16_t {
        const uint16_t denom = (rangeMv == 0) ? 1 : rangeMv;
        const int32_t scaled = static_cast<int32_t>(voltageMv - minMv) * ySpan / denom;
        const int16_t yValue = static_cast<int16_t>(graphY + ySpan - scaled);
        return clampY(yValue);
    };

    const uint16_t prevIndex = sampleStart;
    int16_t prevX = graphX;
    int16_t prevY = voltageToY(samples[prevIndex].voltageMv);

    for (uint16_t i = 1; i < sampleCount; ++i) {
        const uint16_t sampleIndex = static_cast<uint16_t>((sampleStart + i) % BatteryCalibrationSampler::kMaxSamples);
        const int16_t currX = static_cast<int16_t>(graphX + (static_cast<int32_t>(i) * xSpan) / maxIndex);
        const int16_t currY = voltageToY(samples[sampleIndex].voltageMv);
        display->drawLine(prevX, prevY, currX, currY);
        prevX = currX;
        prevY = currY;
    }
}

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
    const bool hasBattery = powerStatus && powerStatus->getHasBattery();
    if (hasBattery) {
        const int batV = powerStatus->getBatteryVoltageMv() / 1000;
        const int batCv = (powerStatus->getBatteryVoltageMv() % 1000) / 10;
        const int batMv = powerStatus->getBatteryVoltageMv(); //just for debug use mV
        //snprintf(voltageStr, sizeof(voltageStr), "%01d.%02dV", batV, batCv);
        snprintf(voltageStr, sizeof(voltageStr), "%04dmV", batMv); //just for debug use mV
        snprintf(percentStr, sizeof(percentStr), "%3d%%", powerStatus->getBatteryChargePercent());
    } else {
        snprintf(voltageStr, sizeof(voltageStr), "USB");
        snprintf(percentStr, sizeof(percentStr), "USB");
    }

    const int lineY = graphics::getTextPositions(display)[1];
    display->drawString(x, lineY, voltageStr);
    display->drawString(x + SCREEN_WIDTH - display->getStringWidth(percentStr), lineY, percentStr);

    int16_t graphX = 0;
    int16_t graphY = 0;
    int16_t graphW = 0;
    int16_t graphH = 0;
    computeGraphBounds(display, x, y, graphX, graphY, graphW, graphH);

    if (!hasBattery) {
        if (graphH > 0) {
            const char *placeholder = "No battery";
            const int16_t textX = static_cast<int16_t>(graphX + (graphW - display->getStringWidth(placeholder)) / 2);
            const int16_t textY = static_cast<int16_t>(graphY + (graphH - FONT_HEIGHT_SMALL) / 2);
            display->drawString(textX, textY, placeholder);
        }
        return;
    }

    const BatteryCalibrationSampler::BatterySample *samples = nullptr;
    uint16_t sampleCount = 0;
    uint16_t sampleStart = 0;
    if (batteryCalibrationSampler) {
        batteryCalibrationSampler->getSamples(samples, sampleCount, sampleStart);
    }

    drawBatteryGraph(display, graphX, graphY, graphW, graphH, samples, sampleCount, sampleStart);
}
#endif