#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/xiaomi/surya

# Assert
TARGET_OTA_ASSERT_DEVICE := surya

# Kernel
BOARD_KERNEL_BASE := 0x00000000
BOARD_RAMDISK_OFFSET := 0x01000000
TARGET_KERNEL_CONFIG := surya_defconfig

# Platform
TARGET_BOARD_PLATFORM_GPU := qcom-adreno618

# Inherit from the proprietary version
-include vendor/xiaomi/surya/BoardConfigVendor.mk
