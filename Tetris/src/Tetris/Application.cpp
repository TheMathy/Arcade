#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <time.h>
#include <cmath>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TextRenderer.h"

static uint32_t s_ScreenWidth = 640;
static uint32_t s_ScreenHeight = 480;
static float s_QuadSize = 0.0f;

class Playfield
{
public:
    Playfield(uint32_t horizontalQuadCount, uint32_t verticalQuadCount)
        : m_VertexBufferID(0), m_IndexBufferID(0), m_VertexArrayID(0), m_BorderDistance(0), m_HorizontalQuadCount(horizontalQuadCount), m_VerticalQuadCount(verticalQuadCount)
    {
        s_QuadSize = s_ScreenHeight / verticalQuadCount;
        m_BorderDistance = (s_ScreenWidth - horizontalQuadCount * s_QuadSize) / 2;


        float vertices[] = {
            (float)m_BorderDistance, 0.0f,
            (float)m_BorderDistance, (float)s_ScreenHeight,
            (float)s_ScreenWidth - (float)m_BorderDistance, 0.0f,
            (float)s_ScreenWidth - (float)m_BorderDistance, (float)s_ScreenHeight
        };

        uint32_t indices[] = {
            0, 1,
            2, 3
        };
 
        glGenBuffers(1, &m_VertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &m_IndexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_VertexArrayID);
        glBindVertexArray(m_VertexArrayID);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }
public:
    uint32_t GetVertexBufferID() const { return m_VertexBufferID; }
    uint32_t GetIndexBufferID() const { return m_IndexBufferID; }
    uint32_t GetVertexArrayID() const { return m_VertexArrayID; }

    uint32_t GetBorderDistance() const { return m_BorderDistance; }
    uint32_t GetHorizontalQuadCount() const { return m_HorizontalQuadCount; }
    uint32_t GetVerticalQuadCount() const { return m_VerticalQuadCount; }
private:
    uint32_t m_VertexBufferID;
    uint32_t m_IndexBufferID;
    uint32_t m_VertexArrayID;

    uint32_t m_BorderDistance;
    uint32_t m_HorizontalQuadCount;
    uint32_t m_VerticalQuadCount;
};

class PieceTable
{
public:
    PieceTable(const Playfield& playfield)
        : m_VertexBufferID(0), m_IndexBufferID(0), m_VertexArrayID(0), m_PieceTable(new uint32_t[playfield.GetHorizontalQuadCount() * playfield.GetVerticalQuadCount()])
    {

        for (uint32_t i = 0; i < playfield.GetHorizontalQuadCount() * playfield.GetVerticalQuadCount(); i++)
        {
            m_PieceTable[i] = 0;
        }

        float vertices[playfield.GetVerticalQuadCount() * playfield.GetHorizontalQuadCount() * 4 * 3];
        uint32_t indices[playfield.GetVerticalQuadCount() * playfield.GetHorizontalQuadCount() * 6];

        float borderDistance = playfield.GetBorderDistance();
        uint32_t vertexPointer = 0;

        for (uint32_t i = 0; i < playfield.GetVerticalQuadCount(); i++)
        {
            for (uint32_t j = 0; j < playfield.GetHorizontalQuadCount(); j++)
            {
                float colorID = vertexPointer % 3;

                vertices[vertexPointer * 12 + 0] = borderDistance + j * s_QuadSize;
                vertices[vertexPointer * 12 + 1] = s_ScreenHeight - i * s_QuadSize;
                vertices[vertexPointer * 12 + 2] = m_PieceTable[vertexPointer];

                vertices[vertexPointer * 12 + 3] = borderDistance + j * s_QuadSize;
                vertices[vertexPointer * 12 + 4] = s_ScreenHeight - (i + 1) * s_QuadSize;
                vertices[vertexPointer * 12 + 5] = m_PieceTable[vertexPointer];

                vertices[vertexPointer * 12 + 6] = borderDistance + (j + 1) * s_QuadSize;
                vertices[vertexPointer * 12 + 7] = s_ScreenHeight - (i + 1) * s_QuadSize;
                vertices[vertexPointer * 12 + 8] = m_PieceTable[vertexPointer];

                vertices[vertexPointer * 12 + 9] = borderDistance + (j + 1) * s_QuadSize;
                vertices[vertexPointer * 12 + 10] = s_ScreenHeight - i * s_QuadSize;
                vertices[vertexPointer * 12 + 11] = m_PieceTable[vertexPointer];

                indices[vertexPointer * 6 + 0] = 0 + 4 * vertexPointer;
                indices[vertexPointer * 6 + 1] = 1 + 4 * vertexPointer;
                indices[vertexPointer * 6 + 2] = 2 + 4 * vertexPointer;
                indices[vertexPointer * 6 + 3] = 2 + 4 * vertexPointer;
                indices[vertexPointer * 6 + 4] = 3 + 4 * vertexPointer;
                indices[vertexPointer * 6 + 5] = 0 + 4 * vertexPointer;

                vertexPointer ++;
            }
        }

        //float vertices[] = {50.0f, 50.0f, 200.0f, 50.0f, 200.0f, 200.0f, 50.0f, 200.0f};
        //uint32_t indices[] = { 0, 1, 2, 2, 3, 0 }; 

        glGenBuffers(1, &m_VertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, playfield.GetVerticalQuadCount() * playfield.GetHorizontalQuadCount() * 4 * 3 * sizeof(float), vertices, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_IndexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, playfield.GetVerticalQuadCount() * playfield.GetHorizontalQuadCount() * 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_VertexArrayID);
        glBindVertexArray(m_VertexArrayID);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1 ,1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)(2 * sizeof(float)));

    }

    ~PieceTable()
    {
        delete[] m_PieceTable;
    }

public:
    void SetQuad(const Playfield& playfield ,uint32_t index, uint32_t colorIndex)
    {
        m_PieceTable[index] = colorIndex;

        float vertices[playfield.GetVerticalQuadCount() * playfield.GetHorizontalQuadCount() * 4 * 3];
        uint32_t indices[playfield.GetVerticalQuadCount() * playfield.GetHorizontalQuadCount() * 6];

        float borderDistance = playfield.GetBorderDistance();
        uint32_t vertexPointer = 0;

        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);

        float data[] = {
            playfield.GetBorderDistance() + (index % playfield.GetHorizontalQuadCount()) * s_QuadSize, s_ScreenHeight - (int)((int)index / playfield.GetHorizontalQuadCount()) * s_QuadSize, (float)colorIndex,
            playfield.GetBorderDistance() + (index % playfield.GetHorizontalQuadCount()) * s_QuadSize, s_ScreenHeight - ((int)((int)index / playfield.GetHorizontalQuadCount()) + 1) * s_QuadSize, (float)colorIndex,
            playfield.GetBorderDistance() + ((index % playfield.GetHorizontalQuadCount()) + 1) * s_QuadSize, s_ScreenHeight - ((int)((int)index / playfield.GetHorizontalQuadCount()) + 1) * s_QuadSize, (float)colorIndex,
            playfield.GetBorderDistance() + ((index % playfield.GetHorizontalQuadCount()) + 1) * s_QuadSize, s_ScreenHeight - (int)((int)index / playfield.GetHorizontalQuadCount()) * s_QuadSize, (float)colorIndex
            };

        glBufferSubData(GL_ARRAY_BUFFER, index * 12 * sizeof(float), sizeof(data), &data);
    }

    uint32_t GetQuad(uint32_t id)
    {
        return m_PieceTable[id];
    }

    void Fall(Playfield playfield, uint32_t row)
    {
        for (uint32_t i = 0; i < row; i++)
        {
            for (uint32_t j = 0; j < 10; j++)
            {
                SetQuad(playfield, row * 10 - i * 10 + j, GetQuad(row * 10 - i * 10 + j - 10));
            }
        }
    }

public:
    uint32_t GetVertexBufferID() const { return m_VertexBufferID; }
    uint32_t GetIndexBufferID() const { return m_IndexBufferID; }
    uint32_t GetVertexArrayID() const { return m_VertexArrayID; }

private:
    uint32_t m_VertexBufferID;
    uint32_t m_IndexBufferID;
    uint32_t m_VertexArrayID;

    uint32_t* m_PieceTable;
};

class Piece
{
public:
    Piece(const float* pieceMap, float colorID)
        : m_ColorID(colorID)
    {   
        for (uint32_t i = 0; i < 9; i++)
        {
            m_PieceMap[i] = pieceMap[i];
        }
    }

    ~Piece()
    {}
public:
    void Respawn()
    {
        m_Position = 15;
        m_Landed = false;
    }

    void Spawn(PieceTable& pieceTable, const Playfield& playfield)
    {
        for (uint32_t i = 0; i < 9; i++)
        {
            if (m_PieceMap[i] == 0)
                continue;
            
            int x = -1 + i % 3;
            int y = 1 - (int)((int)i / 3);

            pieceTable.SetQuad(playfield, m_Position + playfield.GetHorizontalQuadCount() * y * -1 + x, m_ColorID);
        }   
    }

    void Clear(PieceTable& pieceTable, const Playfield& playfield)
    {
        for (uint32_t i = 0; i < 9; i++)
        {
            if (m_PieceMap[i] == 0)
                continue;
            
            int x = -1 + i % 3;
            int y = 1 - (int)((int)i / 3);

            pieceTable.SetQuad(playfield, m_Position + playfield.GetHorizontalQuadCount() * y * -1 + x, 0.0f);
        }
    }

    void Move(PieceTable& pieceTable, const Playfield& playfield)
    {
       // For each quad
       for (uint32_t i = 0; i < 9; i++)
       {
           if (m_PieceMap[i] == 0)
                continue;
            
            bool covered = false;

            for (uint32_t j = 0; j < (int)((8 - i)) / 3; j++)
            {
                if (m_PieceMap[i + 3 * (j + 1)] == 1.0f)
                {
                    covered = true;
                    break;
                }
            }

            if (covered)
                continue;

            int x = -1 + i % 3;
            int y = 1 - (int)((int)i / 3);

            if (m_Position + playfield.GetHorizontalQuadCount() * y * -1 + 10 >= 200)
            {
                m_Landed = true;
                break;
            }

            if (pieceTable.GetQuad(m_Position + playfield.GetHorizontalQuadCount() * y * -1 + x + 10) != 0)
            {
                m_Landed = true;
                break;
            }
       }

        if (!m_Landed)
        {
            Clear(pieceTable, playfield);
            m_Position += 10;
            Spawn(pieceTable, playfield);
        }
    }

    void MoveLeft(PieceTable& pieceTable, const Playfield& playfield)
    {
        for (uint32_t i = 0; i < 9; i++)
        {
            if (m_PieceMap[i] == 0)
                continue;

            bool covered = false;

            for (uint32_t j = 0; j < i % 3; j++)
            {
                if (m_PieceMap[i - (j + 1)] == 1.0f)
                {
                    covered = true;
                    break;
                }             
            }

            if (covered)
                continue;

            int x = -1 + i % 3;
            int y = 1 - (int)((int)i / 3);

            if ((m_Position + x - 1) % 10 == 9)
                return;

            if (pieceTable.GetQuad(m_Position + (int)playfield.GetHorizontalQuadCount() * y * -1 + x - 1) != 0)
                return; 

        }

        Clear(pieceTable, playfield);
        m_Position -= 1;
        Spawn(pieceTable, playfield);
    }

    void MoveRight(PieceTable& pieceTable, const Playfield& playfield)
    {
        for (uint32_t i = 0; i < 9; i++)
        {
            if (m_PieceMap[i] == 0)
                continue;

            bool covered = false;

            for (uint32_t j = 0; j < (8 - i) % 3; j++)
            {
                if (m_PieceMap[i + j + 1] == 1.0f)
                {
                    covered = true;
                    break;
                }             
            }

            if (covered)
                continue;

            int x = -1 + i % 3;
            int y = 1 - (int)((int)i / 3);

            if ((m_Position + x + 1) % 10 == 0)
                return;

            if (pieceTable.GetQuad(m_Position + (int)playfield.GetHorizontalQuadCount() * y * -1 + x + 1) != 0)
                return;           
        }

        Clear(pieceTable, playfield);
        m_Position += 1;
        Spawn(pieceTable, playfield);
    }

    void Rotate(PieceTable& pieceTable, const Playfield& playfield)
    {
        Clear(pieceTable, playfield);

        float tempPieceMap[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; 

        for (uint32_t i = 0; i < 9; i++)
        {
            int x = -1 + i % 3;
            int y = 1 - (int)((int)i / 3);

            int newX = 0 - y;
            int newY = x;
            
            int deltaX = (m_Position + newX) % 10 - (m_Position + x) % 10;

            if (abs(deltaX) > 2)
            {
                Spawn(pieceTable, playfield);
                return;
            }

            if (m_Position + playfield.GetHorizontalQuadCount() * newY * -1 + 10 >= 200)
            {
                Spawn(pieceTable, playfield);
                return;
            }

            if (pieceTable.GetQuad(m_Position + (int)playfield.GetHorizontalQuadCount() * newY * -1 + newX) != 0)
            {
                Spawn(pieceTable, playfield);
                return;
            }

            tempPieceMap[(1 - newY) * 3 + (newX + 1)] = m_PieceMap[i];
        }

        for (uint32_t i = 0; i < 9; i++)
        {
            m_PieceMap[i] = tempPieceMap[i];
        }
    
        Spawn(pieceTable, playfield);
    }

    bool IsLanded() const { return m_Landed; }
    uint32_t GetPosiion() const { return m_Position; }

private:
    float m_ColorID;
    float m_PieceMap[9];
    uint32_t m_Position = 15;
    bool m_Landed = false;
};

class Renderer
{
public:
    Renderer()
        : m_ShaderID(0), m_ProjectionMatrix(glm::ortho(0.0f, (float)s_ScreenWidth, 0.0f, (float)s_ScreenHeight)), m_ColorUniformLocation(0.0f), m_ProjectionMatrixUniformLocation(0.0f)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        m_ShaderID = CreateShader(VertexShaderSource, FragmentShaderSource);
        m_PieceTableShaderID = CreateShader(VertexShaderSourcePieceTable, FragmentShaderSourcePieceTable);

        m_ColorUniformLocation = glGetUniformLocation(m_ShaderID, "u_Color");
        m_ProjectionMatrixUniformLocation = glGetUniformLocation(m_ShaderID, "u_ProjectionMatrix");
        m_TransformationMatrixUniformLocation = glGetUniformLocation(m_ShaderID, "u_TransformationMatrix");

        m_PieceTableProjectionMatrixUniformLocation = glGetUniformLocation(m_PieceTableShaderID, "u_ProjectionMatrix");
    }
    ~Renderer()
    {}

public:
    void Clear(const glm::vec4& color)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderPlayfield(const Playfield& playfield, const glm::vec4& color)
    {
        glBindVertexArray(playfield.GetVertexArrayID());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playfield.GetIndexBufferID());

        glUseProgram(m_ShaderID);

        glm::mat4 m_TransformationMatrix(1.0f);

        glUniform4f(m_ColorUniformLocation, color.r, color.g, color.b, color.a);
        glUniformMatrix4fv(m_ProjectionMatrixUniformLocation, 1, GL_FALSE, &m_ProjectionMatrix[0][0]);
        glUniformMatrix4fv(m_TransformationMatrixUniformLocation, 1, GL_FALSE, &m_TransformationMatrix[0][0]);

        glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, nullptr);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void RenderPieceTable(const PieceTable& pieceTable)
    {
        glBindVertexArray(pieceTable.GetVertexArrayID());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pieceTable.GetIndexBufferID());

        glUseProgram(m_PieceTableShaderID);

        glUniformMatrix4fv(m_PieceTableProjectionMatrixUniformLocation, 1, GL_FALSE, &m_ProjectionMatrix[0][0]);

        glDrawElements(GL_TRIANGLES, 10 * 20 * 6, GL_UNSIGNED_INT, nullptr);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

private:
    static const std::string VertexShaderSource;
    static const std::string FragmentShaderSource;
    static const std::string VertexShaderSourcePieceTable;
    static const std::string FragmentShaderSourcePieceTable;
    
    uint32_t m_ShaderID;
    uint32_t m_PieceTableShaderID;

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_TransformationMatrix;

    int m_ColorUniformLocation;
    int m_ProjectionMatrixUniformLocation;
    int m_TransformationMatrixUniformLocation;

    int m_PieceTableProjectionMatrixUniformLocation;

private:
    uint32_t CreateShader(const std::string& vertexSource, const std::string& fragmentSource)
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
};

const std::string Renderer::VertexShaderSource =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec4 position;\n"
    "\n"
    "uniform mat4 u_ProjectionMatrix;\n"
    "uniform mat4 u_TransformationMatrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = u_ProjectionMatrix * u_TransformationMatrix * position;\n"
    "}\n";

const std::string Renderer::FragmentShaderSource =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "\n"
    "uniform vec4 u_Color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   color = u_Color;\n"
    "}\n";

const std::string Renderer::VertexShaderSourcePieceTable =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec4 position;\n"
    "layout(location = 1) in float colorID;\n"
    "\n"
    "out float out_ColorID;\n"
    "\n"
    "uniform mat4 u_ProjectionMatrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = u_ProjectionMatrix * position;\n"
    "   out_ColorID = colorID;"
    "}\n";

const std::string Renderer::FragmentShaderSourcePieceTable =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "\n"
    "in float out_ColorID;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   switch (int(out_ColorID))\n"
    "   {\n"
    "   case 0:\n"
    "       color = vec4(0.0, 0.0, 0.0, 0.0);\n"
    "       break;\n"
    "   case 1:\n"
    "       color = vec4(0.0, 0.0, 1.0, 1.0);\n"
    "       break;\n"
    "   case 2:\n"
    "       color = vec4(1.0, 0.65, 0.0, 1.0);\n"
    "       break;\n"
    "   case 3:\n"
    "       color = vec4(1.0, 1.0, 0.0, 1.0);\n"
    "       break;\n"
    "   case 4:\n"
    "       color = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "       break;\n"
    "   case 5:\n"
    "       color = vec4(0.5, 0.0, 0.5, 1.0);\n"
    "       break;\n"
    "   case 6:\n"
    "       color = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "       break;\n"
    "   }\n"
    "}\n";

int main()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(s_ScreenWidth, s_ScreenHeight, "Hello There", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize Glad!" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Info:" << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

    Playfield playfield(10, 20);
    PieceTable pieceTable(playfield);

    // J
    float JpieceMap[] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f    
    };

    Piece JPiece(JpieceMap, 1.0f);

    // L
    float LpieceMap[] = {
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f
    };

    Piece LPiece(LpieceMap, 2.0f);

    // O
    float OpieceMap[] = {
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };

    Piece OPiece(OpieceMap, 3.0f);

    // S
    float SpieceMap[] = {
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };

    Piece SPiece(SpieceMap, 4.0f);

    // T
    float TpieceMap[] = {
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f
    };

    Piece TPiece(TpieceMap, 5.0f);

    // Z
    float ZpieceMap[] = {
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f
    };

    Piece ZPiece(ZpieceMap, 6.0f);

    //piece.Spawn(pieceTable, playfield);

    Renderer renderer;

    glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)s_ScreenWidth, 0.0f, (float)s_ScreenHeight);

    Font font("res/fonts/tahoma.fnt", "res/fonts/tahoma.png");
    TextRenderer textRenderer(projectionMatrix);

    TextField textField(glm::vec2(520.0f, 400.0f), 0.2f, "0", font);

    //pieceTable.SetQuad(playfield, 56, 2);
    //pieceTable.SetQuad(playfield, 78, 1);
    //pieceTable.SetQuad(playfield, 156, 1);
    //pieceTable.SetQuad(playfield, 179, 2);

    bool pressedSpace = false;
    bool pressedA = false;
    bool pressedD = false;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();

    //pieceTable.SetQuad(playfield, 1, 1.0f);

    Piece* activePiece = &JPiece;
    float speed = 500.0f;

    srand(time(NULL));

    uint32_t lines = 0;

    while (!glfwWindowShouldClose(window))
    {
        if (activePiece->IsLanded())
        {
            for (uint32_t i = 0; i < 3; i++)
            {
                uint32_t row = (activePiece->GetPosiion() / 10) + i + -1;

                if (row >= 20)
                    continue;

                for (uint32_t j = 0; j < 10; j++)
                {
                    if (pieceTable.GetQuad(row * 10 + j) == 0)
                        break;

                    if (j == 9)
                    {
                        pieceTable.Fall(playfield, row);
                        lines++;
                        textField.SetText(std::to_string(lines));
                    }
                }
            }

            uint32_t randomIndex = rand() % 6;
            switch (randomIndex)
            {
            case 0:         activePiece = &JPiece; break;
            case 1:         activePiece = &LPiece; break;
            case 2:         activePiece = &OPiece; break;
            case 3:         activePiece = &SPiece; break;
            case 4:         activePiece = &TPiece; break;
            case 5:         activePiece = &ZPiece; break;
            }

        activePiece->Respawn();
        activePiece->Spawn(pieceTable, playfield);

        }


        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !pressedSpace)
        {
            activePiece->Rotate(pieceTable, playfield);
            pressedSpace = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS)
        {
            pressedSpace = false;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !pressedA)
        {
            activePiece->MoveLeft(pieceTable, playfield);
            pressedA = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS)
        {
            pressedA = false;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !pressedD)
        {
            activePiece->MoveRight(pieceTable, playfield);
            pressedD = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS)
        {
            pressedD = false;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            speed = 75.0f;
        else
            speed = 500.0f;

        auto endTimepoint = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
        auto duration = end - start;
        double ms = duration * 0.001;

        if (ms > speed)
        {
            activePiece->Move(pieceTable, playfield);
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
            start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        }


        renderer.Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        textRenderer.RenderTextField(textField);

        renderer.RenderPlayfield(playfield, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        renderer.RenderPieceTable(pieceTable);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}