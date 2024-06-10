#include "pvtable.h"

#include "position.h"

namespace Xake{

namespace PVTable{

    std::unordered_map<Key, Move> pventries;

    constexpr std::size_t PVTableSize = 0x100000 * 128;
	constexpr std::size_t PVTableEntries = PVTableSize / sizeof(pventries);
}

void PVTable::init(){

    pventries.reserve(PVTableEntries);
}

void PVTable::insert_entry(const Position &position, Move move){

    pventries[position.get_key()] = move;
}

Move PVTable::prove_move(const Position &position){

    return pventries[position.get_key()];

}

void PVTable::load_pv_line(PVLine &pvLine, DepthSize depth, Position &position){

    Move move = prove_move(position);
    pvLine.depth = 0;

    while(pvLine.depth<depth && move != 0){
        position.do_move(move);
        pvLine.moves[pvLine.depth++] = move;
        move = prove_move(position);
    }

    DepthSize counter = pvLine.depth;
    while(counter>0){
        position.undo_move();
        --counter;
    }

}

}
