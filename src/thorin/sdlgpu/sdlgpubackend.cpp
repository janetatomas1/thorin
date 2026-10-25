#include "thorin/sdlgpu/sdlgpubackend.hpp"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include "thorin/window.hpp"
#include "thorin/log.hpp"

namespace thorin {
    namespace {
        std::weak_ptr<SDL_GPUDevice> sharedDevice;
    }

    SDLGPUBackend::SDLGPUBackend(
        const WindowConfig &config,
        const SDLGPUBackendConfig &gpuConfig
    ) : GPUBackend(config), gpuConfig_(gpuConfig) {}

    void SDLGPUBackend::init() {
        device_ = sharedDevice.lock();
        if (!device_) {
            auto raw = SDL_CreateGPUDevice(
                gpuConfig_.shaderFormat,
                gpuConfig_.debug,
                gpuConfig_.driver.empty() ? nullptr : gpuConfig_.driver.c_str()
            );

            if (!raw) {
                fail("Unable to create GPU device");
            }

            device_ = std::shared_ptr<SDL_GPUDevice>(raw, SDL_DestroyGPUDevice);
            sharedDevice = device_;
        }

        create_window(0);   // SDL_GPU needs no window flag

        if (!SDL_ClaimWindowForGPUDevice(device_.get(), handle_)) {
            fail(std::format("SDL_ClaimWindowForGPUDevice failed: {}", SDL_GetError()));
        }

        SDL_GPUPresentMode presentMode = gpuConfig_.presentMode;
        if (!SDL_WindowSupportsGPUPresentMode(device_.get(), handle_, presentMode)) {
            logger().warn("window {}: present mode {} not supported, using VSYNC", window_->id(), static_cast<int>(presentMode));
            presentMode = SDL_GPU_PRESENTMODE_VSYNC;
        }
        if (!SDL_SetGPUSwapchainParameters(device_.get(), handle_, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode)) {
            fail(std::format("SDL_SetGPUSwapchainParameters failed: {}", SDL_GetError()));
        }

        create_context();
        if (!ImGui_ImplSDL3_InitForSDLGPU(handle_)) {
            fail("ImGui_ImplSDL3_InitForSDLGPU failed");
        }

        ImGui_ImplSDLGPU3_InitInfo info;
        info.Device = device_.get();
        info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(device_.get(), handle_);
        info.MSAASamples = gpuConfig_.msaaSamples;
        if (!ImGui_ImplSDLGPU3_Init(&info)) {
            fail("ImGui_ImplSDLGPU3_Init failed");
        }

        logger().info(
           "window {} '{}' created {}x{}, SDL_GPU {}, scale {}",
           window_->id(),
           config_.title,
           config_.width,
           config_.height,
           SDL_GetGPUDeviceDriver(device_.get()),
           scale_
       );
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
