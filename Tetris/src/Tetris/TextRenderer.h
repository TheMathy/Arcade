#pragma once

#include <string>
#include <vector>

#include <iostream>

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

class Font
{
public:
    Font(const std::string& fontFilePath, const std::string& fontTextruresPath);
    virtual ~Font() = default;

public:
    struct Character
    {
        float charID;
        float xCoord, yCoord;
        float width, height;
        float xOffset, yOffset;
        float xAdvance;
    };

    const Character& GetCharacter(uint32_t charID) const;
    const std::string& GetTexturesPath() const { return m_TexturesPath; }

private:
    std::vector<Character> m_Characters;
    std::string m_TexturesPath;

private:
    void ParseFondFile(const std::string& fontFilePath, std::vector<Character>& outCharacters);
};



class TextField
{
public:
    TextField(const glm::vec2& position, float scale, const std::string& text, const Font& font);
    ~TextField();

public:
    const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }

    const std::string& GetText() const { return m_Text; }
    void SetText(const std::string& text);

    uint32_t GetVertexBufferID() const { return m_VertexBufferID; }
    uint32_t GetIndexBufferID() const { return m_IndexBufferID; }
    uint32_t GetVertexArrayID() const { return m_VertexArrayID; }
    uint32_t GetTextureID() const { return m_TextureID; }

private:
    glm::mat4 m_ModelMatrix;
    std::string m_Text;
    const Font& m_Font;

    uint32_t m_VertexBufferID;
    uint32_t m_IndexBufferID;
    uint32_t m_VertexArrayID;
    uint32_t m_TextureID;

private:
    void GenerateVerticesAndIndices(float* vertices, uint32_t* indices);
    uint32_t CreateTexture(const std::string& texturePath);
};



class TextRenderer
{
public:
    TextRenderer(const glm::mat4& projectionMatrix);
    ~TextRenderer();

public:
    void RenderTextField(const TextField& textField);

private:
    glm::mat4 m_ProjectionMatrix;

    uint32_t m_ShaderID;
    int m_ProjectionMatrixUniformLocation;
    int m_ModelMatrixUniformLocation;

    static const std::string s_VertexShaderSource;
    static const std::string s_FragmentShaderSource;

private:
    uint32_t CreateShader(const std::string& vertexSource, const std::string& fragmentSource);
};