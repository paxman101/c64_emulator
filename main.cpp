#include <iostream>
#include <thread>

extern "C" {
#include "cpu.h"
#include "memory.h"
}

static uint8_t *getMemory(uint16_t address) {
    static auto *mem_arr = new uint8_t[1 << 16];

    return &mem_arr[address];
}

static void threadSleep(double sec) {
    std::this_thread::sleep_for(std::chrono::duration<double>(sec));
}

int main() {
    initMemory(getMemory);

    loadBinFile(PROJECT_ROOT"/6510_Emulator/testing/nestest.nes", 0x8000, 0x0010, 0x4000);
    loadBinFile(PROJECT_ROOT"/6510_Emulator/testing/nestest.nes", 0xC000, 0x0010, 0x4000);

    uint8_t *mem = getMemoryPtr(INT_RESET+1);
    *mem = 0xC0;
    *(mem - 1) = 0x00;

    setSleepFunction(threadSleep);
    //setCPUFreq(1000000);
    initCPU();
    std::thread cpu_thread{runLoop, stdout};
    cpu_thread.join();

    return 0;
}
