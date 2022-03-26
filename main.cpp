#include <iostream>
#include <thread>

extern "C" {
#include "cpu.h"
#include "6510_Emulator/memory.h"
}
#include "memory.h"
#include "colorram.h"

static ColorRam cram{};
static Vic vic{};
static Sid sid{};
static Cia1 cia1{};
static Cia2 cia2{};
static Memory mem{};

static uint8_t getMemoryVal(uint16_t address) {
    return mem.getValue(address);
}

static void setMemoryVal(uint16_t address, uint8_t val) {
    mem.setValue(address, val);
}

static void threadSleep(double sec) {
    vic.run();
    std::this_thread::sleep_for(std::chrono::duration<double>(sec));
}

static void setPtrs() {
    vic.setCRAM(&cram);
    vic.setMemory(&mem);
    mem.setCRAM(&cram);
    mem.setCIAs(&cia1, &cia2);
    mem.setSid(&sid);
    mem.setVic(&vic);
}

int main() {
    initMemoryFuncs(getMemoryVal, setMemoryVal);
    setPtrs();

    setSleepFunction(threadSleep);
    setCPUFreq(10000000);
    initCPU();
    std::thread cpu_thread{runLoop, stdout};
    cpu_thread.join();

    return 0;
}
