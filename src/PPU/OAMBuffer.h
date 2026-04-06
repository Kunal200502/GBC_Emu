#pragma once
#include <queue>
#include <stdint.h>
#include <iostream>

// compare function for the priority queue
struct Compare{
    bool operator()(std::pair<uint8_t, uint8_t> a, std::pair<uint8_t, uint8_t> b){
        return a.second > b.second;
    }
};

using OAM_Queue = std::priority_queue<std::pair<uint8_t, uint8_t>, std::vector<std::pair<uint8_t, uint8_t>>, Compare>;

class OAM_Buffer_Snapshot{
    public:
        int size;
        std::vector<std::pair<uint8_t, uint8_t>> pq;
        OAM_Buffer_Snapshot(int, OAM_Queue);
        OAM_Buffer_Snapshot(){}

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & size;
            ar & pq;
        }
};

class OAM_Buffer{
    private:
        OAM_Queue pq;
    public:
        int size;
        OAM_Buffer();
        void push(std::pair<uint8_t, uint8_t>);
        bool check(uint8_t pixelCol);
        std::pair<uint8_t, uint8_t> pop();
        void clear();
        OAM_Buffer_Snapshot createSnapshot();
        void restoreSnapshot(OAM_Buffer_Snapshot*);
};