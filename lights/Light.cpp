/*
 * Copyright (C) 2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LightService"

#include <log/log.h>

#include "Light.h"

#include <fstream>

namespace {


static void write_value(const std::string& path, int value)
{
    std::ofstream file(path);
    if ( file.is_open() )
    {
        file << std::to_string( value );
    }
}

static int read_value( const std::string& path )
{
    int value = 0;
    std::ifstream file(path);
    if ( file.is_open() )
    {
        file >> value;
    }
    return value;
}

static int calculateBrightness(uint32_t color) {
    // We have a white-only LED.
    // According to types.hal we should use the following formula,
    // and, also, the "high" byte should be ignored:
    return ((77*((color>>16)&0x00ff)) +
           (150*((color>>8)&0x00ff)) +
              (29*(color&0x00ff))) >> 8;
}

static int scaleValue(int requested, int min, int max) {
    // We must scale the "requested" (which is 0-255) to the min-max range.
    // This is a simple proportion:
    return ( requested * ( max-min ) ) / 255;
}

// Manage LCD backlight
static void handleLCDBacklight(const LightState& state)
{
    int max_brightness = read_value( "/sys/class/backlight/panel0-backlight/max_brightness" );
    int requested_brightness = calculateBrightness( state.color );
    int brightness = scaleValue(requested_brightness, 0, max_brightness );
    write_value( "/sys/class/backlight/panel0-backlight/brightness", brightness );
}

// Manage LED
static void handleLED(const LightState& state)
{
    if ( ( state.flashMode == Flash::HARDWARE ) ||
         ( state.flashMode == Flash::TIMED ) )
    {   // we have no hardware asssited flash!
        write_value("/sys/class/leds/white/delay_on", state.flashOffMs);
        write_value("/sys/class/leds/white/delay_off", state.flashOnMs);
    }
    else // If we are not blinking, set brightness
    {
        write_value("/sys/class/leds/white/delay_off", 0);
        write_value("/sys/class/leds/white/delay_on", 0);
        int max_brightness = read_value( "/sys/class/leds/white/max_brightness" );
        int requested_brightness = calculateBrightness( state.color );
        int brightness = scaleValue(requested_brightness, 0, max_brightness );
        write_value("/sys/class/leds/white/brightness", brightness);
    }
}

}  // anonymous namespace

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

Return<Status> Light::setLight(Type type, const LightState& state)
{
    switch ( type )
    {
    case Type::ATTENTION:
    case Type::NOTIFICATIONS:
    case Type::BATTERY:
        handleLED( state );
        return Status::SUCCESS;

    case Type::BACKLIGHT:
        handleLCDBacklight( state );
        return Status::SUCCESS;

    default:
        return Status::LIGHT_NOT_SUPPORTED;
    }
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb)
{
    std::vector<Type> types;
    types.push_back( Type::ATTENTION );
    types.push_back( Type::NOTIFICATIONS );
    types.push_back( Type::BATTERY );
    types.push_back( Type::BACKLIGHT );
    _hidl_cb(types);
    return Void();
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android
