#include "thorin/webgpu/backend.hpp"

#include <algorithm>
#include <format>
#include <limits>
#include <span>
#include <string_view>

#include <imgui_impl_sdl3.h>
#include <imgui_impl_wgpu.h>
#include <sdl3webgpu.h>

#include "thorin/window.hpp"
#include "thorin/log.hpp"

namespace thorin::webgpu {
    struct SharedDevice {
        wgpu::Instance instance;
        wgpu::Adapter adapter;
        wgpu::Device device;
    };

    namespace {
        std::weak_ptr<SharedDevice> sharedDevice;
    }

    Backend::Backend(
        const WindowConfig &config,
        const BackendConfig &gpuConfig
    ) : GPUBackend(config), gpuConfig_(gpuConfig) {}

    void Backend::init() {
        shared_ = sharedDevice.lock();
        if (!shared_) {
            shared_ = std::make_shared<SharedDevice>();
            // Lets WaitAny() block on the adapter and device requests below.
            const wgpu::InstanceFeatureName timedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
            wgpu::InstanceDescriptor instanceDescriptor;
            instanceDescriptor.requiredFeatureCount = 1;
            instanceDescriptor.requiredFeatures = &timedWaitAny;
            shared_->instance = wgpu::CreateInstance(&instanceDescriptor);
            if (!shared_->instance) {
                fail("wgpuCreateInstance failed");
            }
            sharedDevice = shared_;
        }
        const wgpu::Instance& instance = shared_->instance;

        create_window(0);   // WebGPU needs no window flag

        surface_ = wgpu::Surface::Acquire(SDL_GetWGPUSurface(instance.Get(), handle_));
        if (!surface_) {
            fail("SDL_GetWGPUSurface failed");
        }

        // The first window picks the adapter, one that can present to its surface.
        if (!shared_->device) {
            wgpu::RequestAdapterOptions adapterOptions;
            adapterOptions.powerPreference = gpuConfig_.powerPreference;
            adapterOptions.backendType = gpuConfig_.backendType;
            adapterOptions.compatibleSurface = surface_;
            instance.WaitAny(
                instance.RequestAdapter(
                    &adapterOptions,
                    wgpu::CallbackMode::WaitAnyOnly,
                    [this](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message) {
                        if (status != wgpu::RequestAdapterStatus::Success) {
                            fail(std::format("RequestAdapter failed: {}", std::string_view(message)));
                        }
                        shared_->adapter = std::move(adapter);
                    }
                ),
                std::numeric_limits<uint64_t>::max()
            );

            wgpu::DeviceDescriptor deviceDescriptor;
            deviceDescriptor.SetUncapturedErrorCallback(
                [](const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message) {
                    logger().error("WebGPU error {}: {}", static_cast<int>(type), std::string_view(message));
                }
            );
            instance.WaitAny(
                shared_->adapter.RequestDevice(
                    &deviceDescriptor,
                    wgpu::CallbackMode::WaitAnyOnly,
                    [this](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message) {
                        if (status != wgpu::RequestDeviceStatus::Success) {
                            fail(std::format("RequestDevice failed: {}", std::string_view(message)));
                        }
                        shared_->device = std::move(device);
                    }
                ),
                std::numeric_limits<uint64_t>::max()
            );
        }

        wgpu::SurfaceCapabilities capabilities;
        if (surface_.GetCapabilities(shared_->adapter, &capabilities) != wgpu::Status::Success || capabilities.formatCount == 0) {
            fail("Surface::GetCapabilities failed");
        }

        wgpu::PresentMode presentMode = gpuConfig_.presentMode;
        const auto presentModes = std::span(capabilities.presentModes, capabilities.presentModeCount);
        if (std::ranges::find(presentModes, presentMode) == presentModes.end()) {
            logger().warn("window {}: present mode {} not supported, using Fifo", window_->id(), static_cast<int>(presentMode));
            presentMode = wgpu::PresentMode::Fifo;
        }

        int width = 0, height = 0;
        SDL_GetWindowSizeInPixels(handle_, &width, &height);
        surfaceConfig_.device = shared_->device;
        surfaceConfig_.format = capabilities.formats[0];
        surfaceConfig_.width = static_cast<uint32_t>(width);
        surfaceConfig_.height = static_cast<uint32_t>(height);
        surfaceConfig_.presentMode = presentMode;
        surface_.Configure(&surfaceConfig_);

        create_context();
        if (!ImGui_ImplSDL3_InitForOther(handle_)) {
            fail("ImGui_ImplSDL3_InitForOther failed");
        }

        ImGui_ImplWGPU_InitInfo info;
        info.Device = shared_->device.Get();
        info.RenderTargetFormat = static_cast<WGPUTextureFormat>(surfaceConfig_.format);
        info.PipelineMultisampleState.count = gpuConfig_.msaaSamples;
        if (!ImGui_ImplWGPU_Init(&info)) {
            fail("ImGui_ImplWGPU_Init failed");
        }

        wgpu::AdapterInfo adapterInfo;
        shared_->adapter.GetInfo(&adapterInfo);
        logger().info(
           "window {} '{}' created {}x{}, WebGPU on {}, scale {}",
           window_->id(),
           config_.title,
           config_.width,
           config_.height,
           std::string_view(adapterInfo.description),
           scale_
       );
    }
}
