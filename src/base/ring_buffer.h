// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <inttypes.h>

template<typename T, uint8_t SIZE>
class RingBuffer {
    private:
        T buffer_[SIZE];
        volatile uint8_t head_ = 0;
        volatile uint8_t tail_ = 0;
        volatile uint8_t count_ = 0;
    public:
        int push(const T& item) {
            if (count_ == SIZE) {
                return 1;
            }
            
            buffer_[head_] = item;
            head_ = (head_++) % SIZE;
            count_++;
            return 0;
        }
        int pop(T& item) {
            if (count_ == 0) {
                return 1;
            }
            
            item = buffer_[tail_];
            tail_ = (tail_++) % SIZE;
            count_--;
            return 0;
        }
        bool isEmpty() const { return count_ == 0; }
        bool isFull() const { return count_ == SIZE; }
        uint8_t size() const { return count_; }
};
