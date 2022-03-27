#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

extern "C" {
#include "cpu.h"
#include "6510_Emulator/memory.h"
}
#include "memory.h"
#include "colorram.h"
#include "io.h"

static ColorRam cram{};
static Vic vic{};
static Sid sid{};
static Cia1 cia1{};
static Cia2 cia2{};
static Memory mem{};
static Io io{};

static std::mutex mutex;

static uint8_t getMemoryVal(uint16_t address) {
    return mem.getValue(address);
}

static void setMemoryVal(uint16_t address, uint8_t val) {
    mem.setValue(address, val);
}

void runVic();

using our_clock = std::chrono::high_resolution_clock;
using our_ns = std::chrono::duration<double, std::nano>;

static void threadSleep(uint64_t cycles) {
    static auto before = our_clock::now();
    static unsigned total_cycles = 0;
    total_cycles += cycles;
    our_ns time = our_clock::now() - before;
//    vic.run();
//    mutex.unlock();
    std::this_thread::sleep_for(std::chrono::duration<double>(cycles * 1/1000000) - time);
//    std::this_thread::yield();
//    mutex.lock();
    runVic();
//    if (++total_cycles % 18200 == 0 ) {
//        io.update();
//        io.clearScreen();
//    }
    before = our_clock::now();
}

void runCPU(FILE *file) {
//    mutex.lock();
    runLoop(file);
}

static void setPtrs() {
    vic.setCRAM(&cram);
    vic.setMemory(&mem);
    vic.setCIA2(&cia2);
    vic.setIo(&io);
    mem.setCRAM(&cram);
    mem.setCIAs(&cia1, &cia2);
    mem.setSid(&sid);
    mem.setVic(&vic);
}

void runVic() {
//    mutex.lock();
    vic.run();
//    mutex.unlock();
//    std::this_thread::yield();
}

int main(int argc, char *argv[]) {
    initMemoryFuncs(getMemoryVal, setMemoryVal);
    setPtrs();
    io.initScreen();

    setSleepFunction(threadSleep);
    setCPUFreq(10000000);
    initCPU();
//    std::thread cpu_thread{runCPU, stdout};

    runLoop(stdout);

    return 0;
}
