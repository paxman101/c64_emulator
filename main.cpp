#include <iostream>
#include <thread>

extern "C" {
#include "cpu.h"
#include "6510_Emulator/memory.h"
}
#include "memory.h"

static Vic vic{};
static Sid sid{};
static Memory mem{vic, sid};

static uint8_t getMemoryVal(uint16_t address) {
    return mem.getValue(address);
}

static void setMemoryVal(uint16_t address, uint8_t val) {
    mem.setValue(address, val);
}

static void threadSleep(double sec) {
    std::this_thread::sleep_for(std::chrono::duration<double>(sec));
}

int main() {
    initMemoryFuncs(getMemoryVal, setMemoryVal);

    setSleepFunction(threadSleep);
    setCPUFreq(1000000);
    initCPU();
    std::thread cpu_thread{runLoop, stdout};
    cpu_thread.join();

    return 0;
}
