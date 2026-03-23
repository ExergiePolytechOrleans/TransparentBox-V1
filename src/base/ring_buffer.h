#pragma once
#include <inttypes.h>

template<typename T, uint8_t SIZE>
class ring_buffer {
    private:
        T _buffer[SIZE];
        volatile uint8_t _head = 0;
        volatile uint8_t _tail = 0;
        volatile uint8_t _count = 0;
    public:
        int push(const T& item) {
            if (_count == SIZE) {
                return 1;
            }
            
            _buffer[_head] = item;
            _head = (_head++) % SIZE;
            _count++;
            return 0;
        }
        int pop(T& item) {
            if (_count == 0) {
                return 1;
            }
            
            item = _buffer[_tail];
            _tail = (_tail++) % SIZE;
            _count--;
            return 0;
        }
        bool is_empty() const { return _count == 0;}
        bool is_full() const { return _count == SIZE;}
        uint8_t size() const { return _count;}
};