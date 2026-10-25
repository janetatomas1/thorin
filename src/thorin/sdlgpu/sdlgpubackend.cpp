#include "thorin/sdlgpu/sdlgpubackend.hpp"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include "thorin/window.hpp"
#include "thorin/log.hpp"
#include "thorin/assert.hpp"

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
        THORIN_DEBUG_ASSERT(handle_ != nullptr, "SDLGPUBackend::destroy called with no window handle");
        THORIN_DEBUG_ASSERT(device_.get() != nullptr, "SDLGPUBackend::destroy called with no gpu device");

        logger().info("window {} '{}' closed", window_->id(), config_.title);

        // Frames still in flight may use the textures and buffers released below.
        SDL_WaitForGPUIdle(device_.get());

        // The backends and the context are per window; shut them down while this window's context is current.
        make_current();
        ImGui_ImplSDLGPU3_Shutdown();
        if (backgroundTexture_ != nullptr) {
            SDL_ReleaseGPUTexture(device_.get(), backgroundTexture_);
            backgroundTexture_ = nullptr;
        }
        destroy_context();

        SDL_ReleaseWindowFromGPUDevice(device_.get(), handle_);
        SDL_DestroyWindow(handle_);
        // Last: the device goes with the last window that holds it.
        device_.reset();
    }

    void SDLGPUBackend::update(Widget *rootWidget) {
        make_current();

        ImGui_ImplSDLGPU3_NewFrame();
        build_frame(rootWidget);

        ImDrawData* drawData = ImGui::GetDrawData();
        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device_.get());
        if (commandBuffer == nullptr) {
            logger().error("window {}: SDL_AcquireGPUCommandBuffer failed: {}", window_->id(), SDL_GetError());
            return;
        }

        // Null without an error while the window is minimized or the swapchain is not ready; the frame is skipped.
        SDL_GPUTexture* swapchainTexture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, handle_, &swapchainTexture, nullptr, nullptr)) {
            logger().error("window {}: SDL_WaitAndAcquireGPUSwapchainTexture failed: {}", window_->id(), SDL_GetError());
        }

        const bool minimized = drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f;
        if (swapchainTexture != nullptr && !minimized) {
            // Uploads vertices and indices; has to run before the render pass begins.
            ImGui_ImplSDLGPU3_PrepareDrawData(drawData, commandBuffer);

            SDL_GPUColorTargetInfo target{};
            target.texture = swapchainTexture;
            target.load_op = SDL_GPU_LOADOP_CLEAR;
            target.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &target, 1, nullptr);
            ImGui_ImplSDLGPU3_RenderDrawData(drawData, commandBuffer, renderPass);
            SDL_EndGPURenderPass(renderPass);
        }

        // Presents the swapchain texture, if one was acquired.
        SDL_SubmitGPUCommandBuffer(commandBuffer);
    }

    ImTextureID SDLGPUBackend::upload_background() {
        return ImTextureID_Invalid;
    }
}
