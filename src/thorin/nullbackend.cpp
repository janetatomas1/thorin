#include "thorin/nullbackend.hpp"

namespace thorin {
    NullBackend::NullBackend(
        const WindowConfig &config
    ) : GPUBackend(config) {}
}
