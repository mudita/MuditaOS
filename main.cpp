#include <iostream>
#include <memory>
#include "SystemManager/SystemManager.hpp"






int main() {
    std::cout << "Hello, World!" << std::endl;

    auto sysmgr = std::make_shared<core::SystemManager>(20000,5000);

    sysmgr->StartSystem();


    cpp_freertos::Thread::StartScheduler();

    return 0;
}