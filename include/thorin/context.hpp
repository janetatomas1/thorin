
#pragma once

namespace thorin {
    enum GpuBackend {
        OpenGL = 0,
        Vulkan = 1,
        WebGPU = 2,
        RuntimeSelected = 3
    };

    template <GpuBackend gpuBackend = OpenGL>
    struct Context {
        static constexpr GpuBackend GPU_BACKEND = gpuBackend;
    };
}
