#include "GLCall.h"

#include <glad/glad.h>

#include <iostream>

namespace RocketEngine
{
    void GLClearError()
    {
        while (glGetError() != GL_NO_ERROR);
    }

    bool GLLogCall(const char* function, const char* file, int line)
    {
        while (GLenum error = glGetError())
        {
            std::cout << "[OpenGL Error] (" << error << "): " << function << " " << ":" << line << std::endl;
            return false;
        }
        return true;
    }
}