/**
 * @file        utils.hpp
 * 
 * @brief       Utility functions, inclusions, and definitions.
 * 
 * @author      Filippo Maggioli (maggioli.filippo@gmail.com)
 * 
 * @date        2026-03-30
 */
#pragma once

// MSVC has std::min/std::max defined as macros by default.
// This is nasty, and can be avoided with this macro
#define NOMINMAX


// Basic inclusions always useful
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>


// GLM is used everywhere, it makes sense to include it here
#include <glm/glm.hpp>