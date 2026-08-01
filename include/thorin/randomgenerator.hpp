#pragma once
#include <random>
#include <cstdint>

namespace thorin {

    class RandomGenerator {
        std::mt19937_64 engine_;
    public:
        RandomGenerator() {
            std::random_device rd;
            engine_.seed(rd());
        }

        uint64_t random() {
            return engine_();
        }
    };

}
