#ifndef AKERBELTZ_TEST_HELPERS_H
#define AKERBELTZ_TEST_HELPERS_H

#include <initializer_list>
#include <mutex>

#include "attacks.h"
#include "bitboards.h"
#include "evaluate.h"
#include "position.h"

namespace Akerbeltz::TestHelpers {

inline Bitboard squares(std::initializer_list<Square64> list) {
    Bitboard board = Bitboard{0};
    for (auto sq : list) {
        board |= (ONE << sq);
    }
    return board;
}

inline void init_attacks_once() {
    static std::once_flag once;
    std::call_once(once, [] { Attacks::init(); });
}

inline void init_engine_once() {
    static std::once_flag once;
    std::call_once(once, [] {
        Position::init();
        Attacks::init();
    });
}

inline void init_evaluate_once() {
    static std::once_flag once;
    std::call_once(once, [] { Evaluate::init(); });
}

}  // namespace

#endif  // AKERBELTZ_TEST_HELPERS_H
