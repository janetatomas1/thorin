#include "thorin/sdlgpu/sdlgpubackend.hpp"

namespace thorin {
    namespace {
        std::weak_ptr<SDL_GPUDevice> device;
    }

    SDLGPUBackend::SDLGPUBackend(
        const WindowConfig &config,
        const SDLGPUBackendConfig &gpuConfig
    ) : GPUBackend(config), gpuConfig_(gpuConfig) {}

    void SDLGPUBackend::init() {
    }

    void SDLGPUBackend::destroy() {
    }

    void SDLGPUBackend::update(Widget *rootWidget) {
    }

    ImTextureID SDLGPUBackend::upload_background() {
        return ImTextureID_Invalid;
    }

    void SDLGPUBackend::make_current() {
        GPUBackend::make_current();
    }
}
