#include "OAMBuffer.h"

OAM_Buffer_Snapshot::OAM_Buffer_Snapshot(OAM_Queue pq_state){
    while(!pq_state.empty()){
        pq.push_back(pq_state.top());
        pq_state.pop();
    }
}


OAM_Buffer::OAM_Buffer(){
}

void OAM_Buffer::push(std::pair<uint8_t, uint8_t> object){
    if(pq.size() == 10){
        return;
    }
    pq.push(object);
}

bool OAM_Buffer::check(uint8_t pixelCol){
    if(pq.empty()){
        return false;
    }
    while(!pq.empty() && pq.top().second < pixelCol){
        pq.pop();
    }
    uint8_t x = pq.top().second;
    return (x-8 <= pixelCol && x > pixelCol);
}

void OAM_Buffer::clear(){
    while(!pq.empty()){
        pq.pop();
    }
}

std::pair<uint8_t, uint8_t> OAM_Buffer::pop(){
    std::pair<uint8_t, uint8_t> output = pq.top();
    pq.pop();
    return output;
}

OAM_Buffer_Snapshot OAM_Buffer::createSnapshot(){
    OAM_Buffer_Snapshot oam_buffer_snapshot(pq);
    return oam_buffer_snapshot;
}

void OAM_Buffer::restoreSnapshot(OAM_Buffer_Snapshot* snapshot){
    for(std::pair<uint8_t, uint8_t> item: snapshot->pq){
        pq.push(item);
    }
}