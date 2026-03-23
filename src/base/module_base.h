#pragma once
#include "base/task.h"

class module_base
{
public:
    virtual int push(const Task& task) = 0;
};