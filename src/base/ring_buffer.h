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
            // DO NOT use head_++ this creates an incrementation bug where head_ never goes above 0
            head_ = (head_ + 1) % SIZE;
            count_++;
            return 0;
        }
        int pop(T& item) {
            if (count_ == 0) {
                return 1;
            }
            
            item = buffer_[tail_];
            // DO NOT use tail_++ this creates an incrementation bug where tail_ never goes above 0
            tail_ = (tail_ + 1) % SIZE;
            count_--;
            return 0;
        }
        bool isEmpty() const { return count_ == 0; }
        bool isFull() const { return count_ == SIZE; }
        uint8_t size() const { return count_; }
};
