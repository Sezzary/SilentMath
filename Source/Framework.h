#pragma once

// =========
// LIBRARIES
// =========

// Standard
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <functional>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <stack>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
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

// Resources
#include "Utils/Debug.h"
//#include "Math/Math.h"

namespace Silent
{
    // =========
    // CONSTANTS
    // =========

    constexpr int NO_VALUE = -1;
}

using namespace Silent;
using namespace Silent::Debug;
