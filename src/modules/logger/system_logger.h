// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>
#include "custom_types.h"
#include "flags.h"
#include "base/ring_buffer.h"
#include "base/task.h"
#include "base/module_base.h"
#include "data/config_store.h"

class SystemLogger {
private:
    HardwareSerial *output_;
    int printMessage(String prefix, String message);
public:
    SystemLogger(HardwareSerial *output);
    ~SystemLogger();
    #ifdef INFO
    int info(String message);
    int dumpConfig();
    #endif
    
    #ifdef WARN
    int warn(String message);
    #endif
    
    #ifdef ERROR
    int error(String message);
    #endif
    
    #ifdef DEBUG
    int debug(String message);
    #endif

    #ifdef DEEP_DEBUG
    int deepDebug(String message);
    #endif
};
