// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <Arduino.h>
#include "flags.h"
#include "base/ring_buffer.h"
#include "base/task.h"
#include "base/module_base.h"

class system_logger
{
private:
    HardwareSerial *_output;
    int print_message(String pre, String message);
public:
    system_logger(HardwareSerial *output);
    ~system_logger();
    #ifdef INFO
    int info(String message);
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
    int deep_debug(String message);
    #endif
};

system_logger::system_logger(HardwareSerial *output)
{
    _output = output;
}

system_logger::~system_logger()
{
}

int system_logger::print_message(String pre, String message) {
    if (_output->availableForWrite()) {
        _output->print(millis());
        _output->print(pre);
        _output->println(message);
        return 0;
    }
    return 1;
}

#ifdef INFO
int system_logger::info(String message) {
    return this->print_message(" [INFO] ", message);
}
#endif

#ifdef WARN
int system_logger::warn(String message) {
    return this->print_message(" [WARNING] ", message);
}
#endif

#ifdef ERROR
int system_logger::error(String message) {
    return this->print_message(" [ERROR] ", message);
}
#endif

#ifdef DEBUG
int system_logger::debug(String message) {
    return this->print_message(" [DEBUG] ", message);
}
#endif

#ifdef DEEP_DEBUG
int system_logger::deep_debug(String message) {
    return this->print_message(" [DEEP_DEBUG] ", message);
}
#endif