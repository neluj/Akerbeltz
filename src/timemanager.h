#ifndef INCLUDE_TIMEMANAGER_H
#define INCLUDE_TIMEMANAGER_H

#include <chrono>
#include <cmath> 
#include <optional>
#include <algorithm>

namespace Akerbeltz {

class TimeManager {

public:
    using Clock     = std::chrono::steady_clock;
    using Ms        = std::chrono::milliseconds;
    using TimePoint = Clock::time_point;
    using Duration  = Clock::duration;

    struct BudgetParams{
        std::optional<Ms> moveTimeMs, colorTimeMs, incMs;
        std::optional<int> movesToGo;
        std::optional<int> ply;
        Ms  overhead  {Ms(5)}; 
    };

    static inline TimePoint now_tp() { return Clock::now(); }

    inline void mark_start() { startPoint = now_tp(); started = true; }

    inline Ms elapsed_ms() const { return std::chrono::duration_cast<Ms>(now_tp() - startPoint); }

    inline void on_iteration_finished(uint64_t iterNodes, uint64_t prevIterNodes){
        if (!prevIterNodes) return;
        const double r = double(iterNodes) / double(prevIterNodes);
        ema_growth = ema_growth ? 0.7*ema_growth + 0.3*r : r;
    }
 
    inline bool out_of_time() const { return now_tp() >= deadline; }

    bool enough_time_for_next_iteration(Ms last_ms) const;

    std::optional<Ms> remaining_ms() const;

    void allocate_budget(const BudgetParams &params);

private:

    TimePoint startPoint     {};
    bool started = false;
    TimePoint deadline  {TimePoint::max()};
    double ema_growth = 0.0;

};
} // namespace Akerbeltz
#endif