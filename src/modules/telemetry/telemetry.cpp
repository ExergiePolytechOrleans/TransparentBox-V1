// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "telemetry.h"
#include "base/router.h"
#include "data/general_store.h"

int Telemetry::push(const Task &task) { return queue_.push(task); }

Telemetry::Telemetry(HardwareSerial* data_stream) : logger_(nullptr), data_stream_(data_stream) {}

Telemetry::Telemetry(HardwareSerial* data_stream,SystemLogger *logger) : logger_(logger), data_stream_(data_stream) {}

Telemetry::~Telemetry() {}

int Telemetry::init() {
    data_stream_->begin(115200);
    lora_header_.source_ = 0x01;
    lora_header_.dest_ = 0x02;
    return 0;
}

int Telemetry::loop() {
    unsigned long now = millis();
    if (now > last_sent_ + update_interval_) {
        TelemetryPacket2 packet;
        tengGlobalRead(packet.teng);
        vbatGlobalRead(packet.vbat);
        GpsData gps;
        gpsGlobalRead(gps);
        packet.time_stamp = gps.time_;
        packet.lat = gps.lat_.value_;
        packet.lng = gps.lng_.value_;
        packet.speed = gps.speed_.value_;
        lora_header_.size_ = sizeof(packet);
        lora_header_.crc16_ = crc16_ccitt((uint8_t*)&packet, sizeof(packet));
        lora_header_.version_ = 2;
        uart_header_.size_ = sizeof(packet) + sizeof(lora_header_);
        
        if (data_stream_->availableForWrite()) {
            data_stream_->write((uint8_t*)&uart_header_, sizeof(uart_header_));
            data_stream_->write((uint8_t*)&lora_header_, sizeof(lora_header_));
            data_stream_->write((uint8_t*)&packet, sizeof(packet));
        }
        last_sent_ = millis();
    }
    return 0;
}