#include "SimpleSignal.hpp"
#include "HazardPointer.hpp"
#include "SessionManager.hpp"

WRRMMap g_hpmap; // Concurrent map through Hazard Pointers

int main() {
    Simple::SessionManager<int> sess_mgr;
    Simple::Signal<void()> events_logger;
    // TODO: implement whatever is needed

    return 0;
}