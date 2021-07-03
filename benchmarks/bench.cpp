#include "benchmark/benchmark.h"
#include "SimpleSignal/SimpleSignal.hpp"
#include "HPHashMap/HazardPointer.hpp"
#include <mutex>
#include <cmath>
#define IGNORE_RETURN(expr) static_cast<void>(expr)

double global = 0;
[[gnu::noinline]] void cdummy_cb() { global += sqrt(15) + pow(10, -3); } // Some "complex" callback

static void BM_SignalEmit3(benchmark::State& state) {
    Simple::Signal<decltype(cdummy_cb)> signal(3);
    IGNORE_RETURN(signal.connect(cdummy_cb)); // Connect callback 1
    IGNORE_RETURN(signal.connect(cdummy_cb)); // Connect callback 2
    IGNORE_RETURN(signal.connect(cdummy_cb)); // Connect callback 3
    for (auto _ : state) signal.emit();
}
BENCHMARK(BM_SignalEmit3);

static void BM_DirectCbCall3(benchmark::State& state) {
    for (auto _ : state) { cdummy_cb(); cdummy_cb(); cdummy_cb(); }
}
BENCHMARK(BM_DirectCbCall3);

static void BM_MapUpdate(benchmark::State& state) {
    std::map<int,int> mymap;
    for (auto _ : state) mymap[10] = 15;
}
BENCHMARK(BM_MapUpdate);

static void BM_MapLookup(benchmark::State& state) {
    std::map<int,int> mymap;
    mymap[10] = 15;
    for (auto _ : state) mymap[10];
}
BENCHMARK(BM_MapLookup);

static void BM_HPMapUpdate(benchmark::State& state) {
    WRRMMap mymap;
    for (auto _ : state) mymap.Update(10, 15);
}
BENCHMARK(BM_HPMapUpdate);

static void BM_HPMapLookup(benchmark::State& state) {
    WRRMMap mymap;
    mymap.Update(10, 15);
    for (auto _ : state) mymap.Lookup(10);
}
BENCHMARK(BM_HPMapLookup);

static void BM_LockMapUpdate(benchmark::State& state) {
    std::mutex mtx;
    std::map<int,int> mymap;
    for (auto _ : state) { mtx.lock(); mymap[10] = 15; mtx.unlock(); }
}
BENCHMARK(BM_LockMapUpdate);

static void BM_LockMapLookup(benchmark::State& state) {
    std::mutex mtx;
    std::map<int,int> mymap;
    mymap[10] = 15;
    for (auto _ : state) { mtx.lock(); mymap[10]; mtx.unlock(); }
}
BENCHMARK(BM_LockMapLookup);

BENCHMARK_MAIN();