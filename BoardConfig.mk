#
# Copyright (C) 2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from sm6125-common
include device/xiaomi/sm6125-common/BoardConfigCommon.mk

DEVICE_PATH := device/xiaomi/laurel_sprout

# A/B
AB_OTA_UPDATER := true

AB_OTA_PARTITIONS += \
    boot \
    dtbo \
    system \
    vbmeta \
    vendor

BOARD_USES_RECOVERY_AS_BOOT := true
TARGET_NO_RECOVERY := true

# Partitions
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 3221225472
BOARD_VENDORIMAGE_PARTITION_SIZE := 1073741824

# Fingerprint
TARGET_SURFACEFLINGER_UDFPS_LIB := //hardware/xiaomi:libudfps_extension.xiaomi
TARGET_USES_FOD_ZPOS := true

# HIDL
DEVICE_MANIFEST_FILE += $(DEVICE_PATH)/manifest.xml
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += $(DEVICE_PATH)/framework_compatibility_matrix.xml

# Recovery
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/etc/fstab.qcom

# Properties
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

# Display
TARGET_SCREEN_DENSITY := 320

# Kernel
TARGET_KERNEL_CONFIG := vendor/laurel_sprout-perf_defconfig

# Inherit from the proprietary version
include vendor/xiaomi/laurel_sprout/BoardConfigVendor.mk
