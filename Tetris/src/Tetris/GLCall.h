#pragma once

#define ASSERT(x) if (!(x)) __builtin_debugtrap();
#define GLCall(x) x

namespace RocketEngine
{
    void GLClearError();
    bool GLLogCall(const char* function, const char* file, int line);
}