#pragma once

#include <vulkan/vulkan.h>

// True iff argv[1] is exactly "--probe-hw".
//
bool
probing_hardware (int argc, char* argv[]);

// Prints each Metal-backed physical device's name behind a successfully
// created instance. Only call this once vkCreateInstance has actually
// succeeded: MoltenVK's vkCreateInstance itself fails with
// VK_ERROR_INCOMPATIBLE_DRIVER when no Metal devices are enumerable, so a
// live instance already guarantees at least one device.
//
void
print_hardware (VkInstance instance);
