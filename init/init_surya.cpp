/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.
   Copyright (C) 2016 The CyanogenMod Project.
   Copyright (C) 2019-2020 The LineageOS Project.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <fstream>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <vector>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using android::init::property_set;

std::vector<std::string> ro_props_default_source_order = {
    "",
    "product.",
    "system.",
    "vendor.",
};

void property_override(char const prop[], char const value[], bool add = true) {
    prop_info *pi;

    pi = (prop_info *)__system_property_find(prop);
    if (pi)
    __system_property_update(pi, value, strlen(value));
    else if (add)
    __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const std::string &source, 
                       const std::string &prop,
                       const std::string &value, 
                       bool product = false) {
	std::string prop_name;

	if (product) {
		prop_name = "ro.product." + source + prop;
	} else {
		prop_name = "ro." + source + "build." + prop;
	}
	property_override(prop_name.c_str(), value.c_str(), false);
}

void set_device_props(const std::string fingerprint, 
                      const std::string description,
                      const std::string brand, 
                      const std::string device, 
                      const std::string model) {
	for (const auto &source : ro_props_default_source_order) {
		set_ro_build_prop(source, "fingerprint", fingerprint);
		set_ro_build_prop(source, "brand", brand, true);
		set_ro_build_prop(source, "device", device, true);
		set_ro_build_prop(source, "model", model, true);
	}
	property_override("ro.build.fingerprint", fingerprint.c_str());
}

void load_device_properties() {
	std::string hwname = GetProperty("ro.boot.hwname", "");
	
	// Set some defaults to avoid empty settings:
	std::string model = "M2007J20CG";
	std::string device = hwname;
	std::string fingerprint = "POCO/surya_global/surya:10/QKQ1.200512.002/V12.0.7.0.QJGMIXM:user/release-keys";
	std::string description = "surya_global-user 10 QKQ1.200512.002 V12.0.7.0.QJGMIXM release-keys";
	
	if (hwname == "surya") {
		model = "M2007J20CG";
		fingerprint = "POCO/surya_global/surya:10/QKQ1.200512.002/V12.0.4.0.QJGMIXM:user/release-keys";
		description = "surya_global-user 10 QKQ1.200512.002 V12.0.4.0.QJGMIXM release-keys";
	} else if (hwname == "karna") {
		model = "M2007J20CI";
		fingerprint = "POCO/karna_global/karna:10/QKQ1.200512.002/V12.0.4.0.QJGMIXM:user/release-keys";
		description = "karna_global-user 10 QKQ1.200512.002 V12.0.4.0.QJGMIXM release-keys";
	}
      
      	set_device_props(fingerprint, 
		         description, 
		         "Poco", 
		         device, 
		         model ); 
}

void set_nfc()
{
	bool enable_nfc = true;
	std::string hwname = GetProperty("ro.boot.hwname", "");

	// Valid options are "karna" and "surya"
	if (hwname == "karna"){
		enable_nfc = false;
	} 
    
        property_override( "ro.hw.nfc", enable_nfc ? "1" : "0" );
}

void set_dalvik_values()
{
	static const long int mem_gigabyte = 1024ull * 1024ull * 1024ull;
	struct sysinfo sys;
	memset( &sys, 0, sizeof(sys ) );
	sysinfo(&sys);

	// Devices with 6Gbytes of RAM:
	if (sys.totalram >= 5ull * mem_gigabyte ){
	        // Copied from phone-xhdpi-6144-dalvik-heap.mk
		property_set("dalvik.vm.heapstartsize", "16m");
		property_set("dalvik.vm.heapgrowthlimit", "256m");
		property_set("dalvik.vm.heapsize", "512m");
		property_set("dalvik.vm.heaptargetutilization", "0.5");
		property_set("dalvik.vm.heapminfree", "8m");
		property_set("dalvik.vm.heapmaxfree", "32m");
	} else if (sys.totalram >= 7ull * mem_gigabyte ) {
		// Copied from phone-xhdpi-8192-dalvik-heap.mk
		property_set("dalvik.vm.heapstartsize", "24m");
		property_set("dalvik.vm.heapgrowthlimit", "256m");
		property_set("dalvik.vm.heapsize", "512m");
		property_set("dalvik.vm.heaptargetutilization", "0.46");
		property_set("dalvik.vm.heapminfree", "8m");
		property_set("dalvik.vm.heapmaxfree", "48m");
	}
}

void vendor_load_properties()
{ 
	load_device_properties();
	set_dalvik_values();
	set_nfc();
}

