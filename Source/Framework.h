#pragma once

// =========
// LIBRARIES
// =========

// Standard
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <stack>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

// FlatBuffers
#include <flatbuffers/flatbuffers.h>

// GLAD
#include <glad/glad.h>

// GLM
#define  GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>

// ImGui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

// json
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_vulkan.h>

// spdlog
#include <spdlog/spdlog.h>

// Vulkan
#include <vulkan/vulkan.hpp>

// =========
// RESOURCES
// =========

#include "Types.h"
#include "Math/Math.h"

using namespace Silent;
using namespace Silent::Math;

#include "Utils/Debug.h"

using namespace Silent::Utils::Debug;
