#include "timemanager.h"


namespace Akerbeltz {


    bool TimeManager::enough_time_for_next_iteration(Ms last_ms) const {
        if (deadline == TimePoint::max()) return true;
        const auto rem = std::chrono::duration_cast<Ms>(deadline - now_tp()).count();
        const double g = ema_growth ? std::clamp(ema_growth, 2.0, 10.0) : 6.0; // clamp 2–10
        const double safety = 1.20; // 20%
        const int64_t need = (int64_t)std::ceil(last_ms.count() * g * safety) + 2;
        return rem > need;
    }

    std::optional<TimeManager::Ms> TimeManager::remaining_ms() const {
        if (deadline == TimePoint::max())
            return std::nullopt; // Infinite
        auto remaining = deadline - now_tp();
        if (remaining < Duration::zero()) remaining = Duration::zero();
        return std::chrono::duration_cast<Ms>(remaining);
    }

   void TimeManager::allocate_budget(const BudgetParams &params)
    {
        if (!started) { mark_start(); }
        
        // 1) Without clock
        if (!params.colorTimeMs.has_value() && !params.moveTimeMs.has_value()) {
            deadline = TimePoint::max();
        }

        // 2) Movetime
        else if (params.moveTimeMs.has_value()) {

            Ms mtms = params.moveTimeMs.value();
            if (mtms > params.overhead) mtms -= params.overhead; else mtms = Ms::zero();

            if (mtms == Ms::zero()) { 
                deadline = now_tp();
            } else {
                deadline = now_tp() + std::chrono::duration_cast<Duration>(mtms);
            }
        }
        else {

            const auto predict_moves_to_go = [](int ply, Ms remain, std::optional<Ms> inc) -> int {
                int base = 35 - (ply / 2);
                base = std::clamp(base, 8, 50);
            
                const int TIME = static_cast<int>(remain.count());
                int adj = (TIME < 10000)  ? +12 :
                          (TIME < 30000)  ? +8  :
                          (TIME < 120000) ? +4  :
                          (TIME < 300000) ? +2  :
                          (TIME < 600000) ?  0  : -2;
            
                if (inc && inc->count() >= 1000) adj -= 4;
            
                return std::clamp(base + adj, 6, 60);
            };

            const int mtg = (params.movesToGo && *params.movesToGo > 0)
                ? *params.movesToGo
                : predict_moves_to_go(params.ply.value_or(0), params.colorTimeMs.value(), params.incMs);
        
            Ms base = params.colorTimeMs.value() / mtg;
            Ms inc  = params.incMs.value_or(Ms{0});
            Ms mtms = base + inc;
                    
            // Minimum safety buffer (preferably >50%):
            Ms remain  = params.colorTimeMs.value();
            Ms reserve = std::max(params.overhead * 2, Ms{50});
                    
            // Only buffer in if we expect >2 steps; for 1–2 steps, avoid aggressive cuts
            if (mtg > 2 && mtms > remain - reserve)
                mtms = remain - reserve;
                    
            // Apply overhead
            if (mtms > params.overhead) mtms -= params.overhead; else mtms = Ms::zero();
            deadline = (mtms == Ms::zero()) ? now_tp() : now_tp() + std::chrono::duration_cast<Duration>(mtms);
        }
    }

}