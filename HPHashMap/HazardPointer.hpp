//
// Created by vfaychuk on 03.07.2021.
//

#ifndef DUMMY_HAZARDPOINTER_HPP
#define DUMMY_HAZARDPOINTER_HPP

#include <map>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>

class HPRecType {
    HPRecType * pNext_;
    // State of the current HP record
    std::atomic_bool active_;
    // Global header of the HP list
    static std::atomic<HPRecType*> pHead_;
    // The length of the list
    static std::atomic_size_t listLen_;
public:
    // Can be used by the thread
    // that acquired it
    void *pHazard_;

    static HPRecType * Head() {
        return pHead_.load();
    }

    // Acquires one hazard pointer
    static HPRecType * Acquire() {
        // Try to reuse a retired HP record
        HPRecType * p = pHead_.load();
        for (; p; p = p->pNext_) {
            bool inactive = false;
            if (p->active_.load() || !p->active_.compare_exchange_weak(inactive, true))
                continue;
            // Got one!
            return p;
        }
        // Increment the list length
        size_t oldLen;
        do {
            oldLen = listLen_.load();
        } while (!listLen_.compare_exchange_weak(oldLen, oldLen + 1));
        // Allocate a new one
        p = new HPRecType;
        p->active_.store(1);
        p->pHazard_ = 0;
        // Push it to the front
        HPRecType * old;
        do {
            old = pHead_.load();
            p->pNext_ = old;
        } while (!pHead_.compare_exchange_weak(old, p));
        return p;
    }
    // Releases a hazard pointer
    static void Release(HPRecType* p) {
        p->pHazard_ = 0;
        p->active_.store(false);
    }
    friend void Scan(HPRecType * head);
    friend class WRRMMap;
};

std::atomic<HPRecType*> HPRecType::pHead_ = 0;
std::atomic_size_t HPRecType::listLen_ = 0;

thread_local std::vector<std::map<int, int>*> rlist;

class WRRMMap {
    std::atomic<std::map<int,int>*> pMap_ = new std::map<int,int>();

    static void Retire(std::map<int, int> *pOld) {
        // Put it in the retired list
        rlist.push_back(pOld);
        if (rlist.size() >= 2) {
            Scan(HPRecType::Head());
        }
    }
public:
    void Update(const int &k, const int &v) {
        std::map<int, int> * pNew = 0;
        std::map<int, int> * pOld;
        do {
            pOld = pMap_;
            delete pNew;
            pNew = new std::map<int,int>(*pOld);
            (*pNew)[k] = v;
        } while (!pMap_.compare_exchange_weak(pOld, pNew));
        Retire(pOld);
    }

    int Lookup(const int &k) {
        HPRecType * pRec = HPRecType::Acquire();
        std::map<int, int> * ptr;
        do {
            ptr = pMap_;
            pRec->pHazard_ = ptr;
        } while (pMap_ != ptr);
        // Save Willy
        int result = (*ptr)[k];
        // pRec can be released now
        // because it's not used anymore
        HPRecType::Release(pRec);
        return result;
    }
};

void Scan(HPRecType * head) {
    // Stage 1: Scan hazard pointers list
    // collecting all non-null pointers
    std::vector<void*> hp;
    while (head) {
        void * p = head->pHazard_;
        if (p) hp.push_back(p);
        head = head->pNext_;
    }
    // Stage 2: sort the hazard pointers
    std::sort(hp.begin(), hp.end(), std::less<>());
    // Stage 3: Search for them!
    auto i = rlist.begin();
    while (i != rlist.end()) { // cppcheck-suppress [invalidContainer]
        if (!std::binary_search(hp.begin(), hp.end(), *i)) {
            // Aha!
            delete *i;
            if (&*i != &rlist.back()) {
                *i = rlist.back();
            }
            // Following is safe - i always points before last element
            rlist.pop_back();
        } else {
            ++i;
        }
    }
}

#endif //DUMMY_HAZARDPOINTER_HPP
