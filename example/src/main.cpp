#include <polymorph/polymorph.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

constexpr const char* WINDOW_TITLE = "Example";

int main()
{
    poly::window<int> window(800, 600, WINDOW_TITLE, 0);

    auto required_layers = std::vector<const char*> { "VK_LAYER_KHRONOS_validation" };
    auto required_device_extensions = std::vector<const char*> { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    poly::vk::context context;
    context.init(WINDOW_TITLE, required_layers, required_device_extensions, window.handle);

    window.start([&]()
    {

    });
    context.cleanup();

    return 0;
}