#include "gtest/gtest.h"
#include "SimpleSignal/SimpleSignal.hpp"

#include <vector>
#include <chrono>
#include <cmath>

class ExpectedException : std::exception {};
class UnexpectedException : std::exception {};
#define IGNORE_RETURN(expr) static_cast<void>(expr)

using namespace Simple;

size_t global = 0;
[[gnu::noinline]] void dummy_cb() { global++; }

TEST(SimpleSignal, InitialSizeIs0) {
    Signal<decltype(dummy_cb)> signal;
    EXPECT_EQ(signal.size(), 0);
}

TEST(SimpleSignal, SizeIncrementsWithAddingCallbacks) {
    Signal<decltype(dummy_cb)> signal;
    for (uint8_t i = 0; i < 0xff; ++i) {
        EXPECT_EQ (signal.size(), i);
        IGNORE_RETURN (signal.connect(&dummy_cb));
    }
}

TEST(SimpleSignal, RemovingExistingCallbackOk) {
    Signal<decltype(dummy_cb)> signal(10);
    auto slot_handle = signal.connect(dummy_cb);
    EXPECT_NO_THROW(signal.disconnect(slot_handle));
}

TEST(SimpleSignal, ConnectingExcessiveSlotThrows) {
    Signal<decltype(dummy_cb)> signal(1);
    IGNORE_RETURN(signal.connect(dummy_cb));
    EXPECT_THROW ({
        try {
            IGNORE_RETURN(signal.connect(dummy_cb));
        } catch (const Signal<decltype(dummy_cb)>::SlotInvalid &e) {
            if (strcmp(e.what(), "Run out of slots") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
}

TEST(SimpleSignal, RemovingOnlyUnexistingCallbackThrows) {
    Signal<decltype(dummy_cb)> signal;
    auto slot_handle = signal.connect(dummy_cb);
    EXPECT_NO_THROW (signal.disconnect(slot_handle));
    EXPECT_THROW ({
        try {
            signal.disconnect(slot_handle);
        } catch (const Signal<decltype(dummy_cb)>::SlotInvalid &e) {
            if (strcmp(e.what(), "No such slot") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
    Signal<decltype(dummy_cb)> other_signal;
    auto broken_slot_handle = other_signal.connect(dummy_cb);
    EXPECT_THROW ({
        try {
            signal.disconnect(broken_slot_handle);
        } catch (const Signal<decltype(dummy_cb)>::SlotInvalid &e) {
            if (strcmp(e.what(), "No such slot") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
}

TEST(SimpleSignal, SizeDecrementsWithRemovingCallbacks) {
    Signal<decltype(dummy_cb)> signal;
    std::vector<decltype(signal)::SlotHandle> slot_handles;
    slot_handles.reserve(0xff);
    for (uint8_t i = 0; i < 0xff; ++i)
        slot_handles.push_back(signal.connect(dummy_cb));
    for (const auto & slot_handle : slot_handles)
        signal.disconnect(slot_handle);
    EXPECT_EQ(signal.size(), 0);
}

TEST(SimpleSignal, SignalInvokesSlot) {
    Signal<decltype(dummy_cb)> signal;
    IGNORE_RETURN(signal.connect(dummy_cb));
    global=0; signal.emit();
    EXPECT_EQ(global, 1);
}

TEST(SimpleSignal, SignalInvokesAllSlots) {
    Signal<decltype(dummy_cb)> signal;
    IGNORE_RETURN(signal.connect(dummy_cb));
    IGNORE_RETURN(signal.connect(dummy_cb));
    IGNORE_RETURN(signal.connect(dummy_cb));
    global=0; signal.emit();
    EXPECT_EQ(global, 3);
}

TEST(SimpleSignal, SignalWorkWithLambdas) {
    size_t variable = 0;
    Signal<void()> signal;
    IGNORE_RETURN(signal.connect([&]() { variable++; }));
    for (uint8_t i = 0; i < 0xff; ++i) {
        EXPECT_EQ(variable, i);
        signal.emit();
    }
}

[[gnu::noinline]] void idummy_cb(int i) { global += i; } // interactive

TEST(SimpleSignal, EmissionPassesParametersToSlot) {
    Signal<decltype(idummy_cb)> signal;
    IGNORE_RETURN(signal.connect(idummy_cb));
    for (uint8_t i = 0; i < 0xff; ++i) {
        global = 0; signal.emit(i);
        EXPECT_EQ(global, i);
    }
}

struct Obj {
    size_t variable;
    [[gnu::noinline]] void dummy_cb() { variable++; }
};

TEST(SimpleSignal, SignalWorkWithObjectReferences) {
    Obj o; o.variable = 0;
    Signal<void()> signal;
    IGNORE_RETURN(signal.connect_slot(o, &Obj::dummy_cb));
    for (uint8_t i = 0; i < 0xff; ++i) {
        EXPECT_EQ(o.variable, i);
        signal.emit();
    }
}

TEST(SimpleSignal, SignalWorkWithObjectPointers) {
    Obj* o = new Obj; o->variable = 0;
    Signal<void()> signal;
    IGNORE_RETURN(signal.connect_slot(o, &Obj::dummy_cb));
    for (uint8_t i = 0; i < 0xff; ++i) {
        EXPECT_EQ(o->variable, i);
        signal.emit();
    }
}

[[gnu::noinline]] void cdummy_cb() { global += sqrt(15) + pow(10, -3); } // complex

size_t count_micros(const std::function<void()> &cbf) {
    using namespace std::chrono;
    auto start = steady_clock::now();
    cbf();
    return duration_cast<microseconds>(steady_clock::now()-start).count();
}

TEST(SimpleSignal, PerformanceDropLessThan30Percent) {
    global = 0;
    Signal<decltype(cdummy_cb)> signal(0x3fffff);
    for (uint32_t i = 0; i < 0x3fffff; ++i) IGNORE_RETURN(signal.connect(cdummy_cb));
    size_t allowed_micros = 1.3f * count_micros([] {
        for (uint32_t i = 0; i < 0x3fffff; ++i) cdummy_cb();
    });
    EXPECT_GE(allowed_micros, count_micros([&signal] { signal.emit(); }));
}
