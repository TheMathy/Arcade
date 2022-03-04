#include "TextRenderer.h"

#include <fstream>
#include <sstream>

Font::Font(const std::string& fontFilePath, const std::string& fontTextruresPath)
    : m_TexturesPath(fontTextruresPath)
{
    ParseFondFile(fontFilePath, m_Characters);
}

const Font::Character& Font::GetCharacter(uint32_t charID) const
{
    for (uint32_t i = 0; i < m_Characters.size(); i++)
        {
            if (m_Characters[i].charID == charID)
                return m_Characters[i];
        }

        std::cout << "Character not defined" << std::endl;
        return m_Characters[0];
}

void Font::ParseFondFile(const std::string& fontFilePath, std::vector<Character>& outCharacters)
{
    std::fstream fileStream(fontFilePath);
    std::string line;

    std::stringstream stringStream;
    std::string parameter = "";

    while (std::getline(fileStream, line))
    {
        stringStream.clear();
        stringStream.str(line);

        stringStream >> parameter;

        if (parameter == "char")
        {
            Character character;

            stringStream >> parameter;
            character.charID = std::stof(parameter.substr(parameter.find('=') + 1));

            stringStream >> parameter;
            character.xCoord = std::stof(parameter.substr(parameter.find('=') + 1));

            stringStream >> parameter;
            character.yCoord = std::stof(parameter.substr(parameter.find('=') + 1));

            stringStream >> parameter;
            character.width = std::stof(parameter.substr(parameter.find('=') + 1));

            stringStream >> parameter;
            character.height = std::stof(parameter.substr(parameter.find('=') + 1));

            stringStream >> parameter;
            character.xOffset = std::stof(parameter.substr(parameter.find('=') + 1));

            stringStream >> parameter;
            character.yOffset = std::stof(parameter.substr(parameter.find('=') + 1));

            stringStream >> parameter;
            character.xAdvance = std::stof(parameter.substr(parameter.find('=') + 1));

            m_Characters.push_back(character);
        }
    }
}



TextField::TextField(const glm::vec2& position, float scale, const std::string& text, const Font& font)
    : m_ModelMatrix(1.0f), m_Text(text), m_Font(font), m_VertexBufferID(0), m_IndexBufferID(0), m_VertexArrayID(0), m_TextureID(0)
{
    m_ModelMatrix = glm::translate(m_ModelMatrix, glm::vec3(position.x, position.y, 0.0f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(scale, scale, 1.0f));

    float vertices[m_Text.size() * 4 * 4];
    uint32_t indices[m_Text.size() * 6];

    GenerateVerticesAndIndices(vertices, indices);

    glGenBuffers(1, &m_VertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, m_Text.size() * 4 * 4 * sizeof(float), vertices, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_IndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Text.size() * 6 * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &m_VertexArrayID);
    glBindVertexArray(m_VertexArrayID);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1 ,2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_TextureID = CreateTexture(m_Font.GetTexturesPath());
}

TextField::~TextField()
{
    glDeleteBuffers(1, &m_VertexBufferID);
    glDeleteBuffers(1, &m_IndexBufferID);
    glDeleteVertexArrays(1, &m_VertexArrayID);
    glDeleteTextures(1, &m_TextureID);
}

void TextField::SetText(const std::string& text)
{
    m_Text = text;

    float vertices[m_Text.size() * 4 * 4];
    uint32_t indices[m_Text.size() * 6];

    GenerateVerticesAndIndices(vertices, indices);

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, m_Text.size() * 4 * 4 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Text.size() * 6 * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TextField::GenerateVerticesAndIndices(float* vertices, uint32_t* indices)
{
    float cursorOffset = 0;

    for (uint32_t i = 0; i < m_Text.size(); i++)
    {
        Font::Character character = m_Font.GetCharacter(m_Text[i]);

        vertices[i * 16 + 0] = cursorOffset + character.xOffset;
        vertices[i * 16 + 1] = 0 - character.yOffset;
        vertices[i * 16 + 2] = character.xCoord / 512;
        vertices[i * 16 + 3] = (512 - character.yCoord) / 512;

        vertices[i * 16 + 4] = cursorOffset + character.xOffset;
        vertices[i * 16 + 5] = 0 - character.yOffset - character.height;
        vertices[i * 16 + 6] = character.xCoord / 512;
        vertices[i * 16 + 7] = (512 - character.yCoord - character.height) / 512;

        vertices[i * 16 + 8] = cursorOffset + character.xOffset + character.width;
        vertices[i * 16 + 9] = 0 - character.yOffset - character.height;
        vertices[i * 16 + 10] = (character.xCoord + character.width) / 512;
        vertices[i * 16 + 11] = (512 - character.yCoord - character.height) / 512;

        vertices[i * 16 + 12] = cursorOffset + character.xOffset + character.width;
        vertices[i * 16 + 13] = 0 - character.yOffset;
        vertices[i * 16 + 14] = (character.xCoord + character.width) / 512;
        vertices[i * 16 + 15] = (512 - character.yCoord) / 512;

        indices[i * 6 + 0] = 0 + 4 * i;
        indices[i * 6 + 1] = 1 + 4 * i;
        indices[i * 6 + 2] = 2 + 4 * i;
        indices[i * 6 + 3] = 2 + 4 * i;
        indices[i * 6 + 4] = 3 + 4 * i;
        indices[i * 6 + 5] = 0 + 4 * i;

        cursorOffset += character.xAdvance;
    }
}

uint32_t TextField::CreateTexture(const std::string& texturePath)
{
    uint32_t id;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;

    m_LocalBuffer = stbi_load(m_Font.GetTexturesPath().c_str(), &m_Width, &m_Height, &m_BPP, 4);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);

    return id;
}



TextRenderer::TextRenderer(const glm::mat4& projectionMatrix)
    : m_ProjectionMatrix(projectionMatrix), m_ShaderID(0), m_ProjectionMatrixUniformLocation(0), m_ModelMatrixUniformLocation(0)
{
    m_ShaderID = CreateShader(s_VertexShaderSource, s_FragmentShaderSource);
    m_ProjectionMatrixUniformLocation = glGetUniformLocation(m_ShaderID, "u_ProjectionMatrix");
    m_ModelMatrixUniformLocation = glGetUniformLocation(m_ShaderID, "u_ModelMatrix");
}

TextRenderer::~TextRenderer()
{
    glDeleteProgram(m_ShaderID);
}

void TextRenderer::RenderTextField(const TextField& textField)
{
    glBindVertexArray(textField.GetVertexArrayID());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textField.GetIndexBufferID());

    glUseProgram(m_ShaderID);
    glUniformMatrix4fv(m_ProjectionMatrixUniformLocation, 1, GL_FALSE, &m_ProjectionMatrix[0][0]);
    glUniformMatrix4fv(m_ModelMatrixUniformLocation, 1, GL_FALSE, &textField.GetModelMatrix()[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textField.GetTextureID());

    glDrawElements(GL_TRIANGLES, textField.GetText().size() * 6, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t TextRenderer::CreateShader(const std::string& vertexSource, const std::string& fragmentSource)
{
    uint32_t id = glCreateProgram();

    uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);

    const char* src = vertexSource.c_str();
    glShaderSource(vs, 1, &src, nullptr);
    src = fragmentSource.c_str();
    glShaderSource(fs, 1, &src, nullptr);

    glCompileShader(vs);
    glCompileShader(fs);

    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    glValidateProgram(id);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return id;
}


const std::string TextRenderer::s_VertexShaderSource =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec4 position;\n"
    "layout(location = 1) in vec2 texCoord;\n"
    "\n"
    "out vec2 v_TexCoord;\n"
    "uniform mat4 u_ProjectionMatrix;\n"
    "uniform mat4 u_ModelMatrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = u_ProjectionMatrix * u_ModelMatrix * position;\n"
    "   v_TexCoord = texCoord;\n"
    "}\n";

const std::string TextRenderer::s_FragmentShaderSource =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "\n"
    "in vec2 v_TexCoord;\n"
    "uniform sampler2D u_Texture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   color = texture(u_Texture, v_TexCoord);\n"
    "}\n";