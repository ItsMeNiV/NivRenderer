#pragma once

#include <iostream>
#include <stdint.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <functional>

#include "OpenGLStarter.h"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

#define SHIFTBITL(x) (1 << x)
#define SHIFTBITR(x) (1 >> x)

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
