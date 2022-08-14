#pragma once

#include "error.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

#define ASSERT_GLFW(expr) { if(!(expr)) { print_error("GLFW", #expr, __FILE__, __LINE__); } }

namespace poly
{
    template <typename T>
    struct window
    {
        GLFWwindow* handle;

        window(int width, int height, const std::string& title, T* user_ptr)
        {
            ASSERT_GLFW(glfwInit());
            
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            handle = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
            ASSERT_GLFW(handle);

            glfwSetWindowUserPointer(handle, user_ptr);
        }

        ~window()
        {
            glfwTerminate();
        }

        void start(std::function<void()> update)
        {
            while(!glfwWindowShouldClose(handle))
            {
                glfwPollEvents();
                update();
            }
        }
    };
}