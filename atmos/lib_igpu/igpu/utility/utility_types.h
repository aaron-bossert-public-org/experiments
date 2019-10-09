
#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace utility
{
    struct aabb
    {
		glm::vec3 min = glm::vec3(0);
		glm::vec3 max = glm::vec3(0);
	};

    //--------------------------------------------------------------------------
    //
    //
    struct rgba8
    {
        unsigned char r = 0, g = 0, b = 0, a = 255;
    };

    //--------------------------------------------------------------------------
    //
    //
    struct ray
    {
		glm::vec3 origin = glm::vec3(0);
        glm::vec3 direction = glm::vec3(0, 0, 1);
    };

    //--------------------------------------------------------------------------
    //
    //
    struct plane
    {
        glm::vec3 n = glm::vec3(0, 0, 1);
        float d = 0;
    };

    //--------------------------------------------------------------------------
    //
    //
    struct frustum
    {
        plane left, right, bottom, top, near, far;
    };

    //--------------------------------------------------------------------------
    //
    //
    struct sphere
    {
		glm::vec3 center = glm::vec3(0);
        float radius = 0;
    };

    //--------------------------------------------------------------------------
    //
    //
    struct viewport
    {
        int32_t x = 0, y = 0;
        int32_t w = 0, h = 0;
    };

    //--------------------------------------------------------------------------
    //
    //
    struct scissor
    {
        int32_t x = 0, y = 0;
        int32_t w = 0, h = 0;
        float zmin = 0, zmax = 0;
    };
}

#include <igpu/utility/utility_types-inl.h>
