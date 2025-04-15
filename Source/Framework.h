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
#include <functional>
#include <future>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <stack>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unordered_set>
#include <variant>
#include <vector>

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>

// spdlog
#include <spdlog/spdlog.h>

namespace Silent
{
    // =========
    // CONSTANTS
    // =========

    constexpr int NO_VALUE = -1;
}

// Resources
#include "Utils/Debug.h"
//#include "Math/Math.h"

using namespace Silent;
//using namespace Silent::Math;
using namespace Silent::Utils::Debug;
