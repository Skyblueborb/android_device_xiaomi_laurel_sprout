/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "UdfpsHandler.laurel_sprout"

#include "UdfpsHandler.h"

#include <android-base/logging.h>
#include <fcntl.h>
#include <fstream>
#include <poll.h>
#include <thread>
#include <unistd.h>

#define COMMAND_NIT 10
#define PARAM_NIT_FOD 1
#define PARAM_NIT_NONE 0

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

static const char* kFodUiPath = "/sys/devices/platform/soc/soc:qcom,dsi-display/fod_ui";

static const char* kFodStatusPath = "/sys/class/touch/tp_dev/fod_status";

static bool readBool(int fd) {
    char c;
    int rc;

    rc = lseek(fd, 0, SEEK_SET);
    if (rc) {
        LOG(ERROR) << "failed to seek fd, err: " << rc;
        return false;
    }

    rc = read(fd, &c, sizeof(char));
    if (rc != 1) {
        LOG(ERROR) << "failed to read bool from fd, err: " << rc;
        return false;
    }

    return c != '0';
}

class LaurelSproutUdfpsHandler : public UdfpsHandler {
  public:
    void init(fingerprint_device_t *device) {
        mDevice = device;

        std::thread([this]() {
            int fd;
            fd = open(kFodUiPath, O_RDONLY);

            if (fd < 0) {
                LOG(ERROR) << "failed to open fd, err: " << fd;
                return;
            }

            int fodStatusFd;
            fodStatusFd = open(kFodStatusPath, O_RDWR);

            struct pollfd fodUiPoll = {
                    .fd = fd,
                    .events = POLLERR | POLLPRI,
                    .revents = 0,
            };

            while (true) {
                int rc = poll(&fodUiPoll, 1, -1);
                if (rc < 0) {
                    LOG(ERROR) << "failed to poll fd, err: " << rc;
                    continue;
                }

                mDevice->extCmd(mDevice, COMMAND_NIT,
                                readBool(fd) ? PARAM_NIT_FOD : PARAM_NIT_NONE);
                if (fodStatusFd >= 0) {
                    write(fodStatusFd, readBool(fd) ? "1" : "0", 1);
                }
            }
        }).detach();
    }

    void onFingerDown(uint32_t /*x*/, uint32_t /*y*/, float /*minor*/, float /*major*/) {
        // nothing
    }

    void onFingerUp() {
        // nothing
    }

    void onAcquired(int32_t result, int32_t vendorCode) {
        if (result == FINGERPRINT_ACQUIRED_GOOD) {
            set(kFodStatusPath, 0);
        } else if (vendorCode == 21 || vendorCode == 23) {
            /*
             * vendorCode = 21 waiting for fingerprint authentication
             * vendorCode = 23 waiting for fingerprint enroll
             */
            set(kFodStatusPath, 1);
        }
    }

    void cancel() {
        // nothing
    }
  private:
    fingerprint_device_t *mDevice;
};

static UdfpsHandler* create() {
    return new LaurelSproutUdfpsHandler();
}

static void destroy(UdfpsHandler* handler) {
    delete handler;
}

extern "C" UdfpsHandlerFactory UDFPS_HANDLER_FACTORY = {
    .create = create,
    .destroy = destroy,
};
