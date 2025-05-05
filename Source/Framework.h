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
#include <cstdlib>
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

// GLM
#define  GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>

// json
#include <nlohmann/json.hpp>

// SDL
#include <SDL3/SDL.h>
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
#include "Utils/Debug.h"

using namespace Silent;
using namespace Silent::Math;
using namespace Silent::Utils::Debug;
