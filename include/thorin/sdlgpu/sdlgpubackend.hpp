#pragma once

#include "thorin/gpubackend.hpp"
#include <SDL3/SDL_gpu.h>

namespace thorin {
    // SDL_GPU settings of an SDLGPUBackend; set through Thorin::set_backend_factory(), shared by every window it makes.
    struct SDLGPUBackendConfig {
        // Driver SDL should use ("vulkan", "direct3d12", "metal"); empty lets SDL pick.
        std::string driver;
        // Turns on the driver's validation layers (SDL_CreateGPUDevice's debug_mode).
        bool debug = false;
        // IMMEDIATE matches GLBackend's swap interval 0, but not every driver supports it; VSYNC always works.
        SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
        // Passed to ImGui_ImplSDLGPU3_InitInfo::MSAASamples.
        SDL_GPUSampleCount msaaSamples = SDL_GPU_SAMPLECOUNT_1;
    };

    // SDL_GPU backend (Vulkan, Metal or D3D12, chosen by SDL): an SDL GPU device and ImGui context per window.
    class SDLGPUBackend: public GPUBackend {
        SDLGPUBackendConfig gpuConfig_;
        std::shared_ptr<SDL_GPUDevice> sharedDevice_;

    protected:
        ImTextureID upload_background() override;

    public:
        SDLGPUBackend(const WindowConfig &config = {}, const SDLGPUBackendConfig &gpuConfig = {});
        void init() override;
        void destroy() override;
        void update(Widget *rootWidget) override;
        void make_current() override;
    };
}
