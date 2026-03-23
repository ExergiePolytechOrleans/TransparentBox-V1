#pragma once
#include "base/task.h"
#include "base/module_base.h"
#include "base/modules.h"
namespace router {
    int send(const Task& task) {
       if (task.target >= MOD_COUNT) {
        return 1;
       } 
       
        module_base* mod = modules[task.target];
        
        if (mod == nullptr) {
            return 1;
        }
        
        return mod->push(task);
    }
    
    int send(module_id target, task_type type, uint32_t data=0) {
        Task t{target, type, data};
        return send(t);
    }
}