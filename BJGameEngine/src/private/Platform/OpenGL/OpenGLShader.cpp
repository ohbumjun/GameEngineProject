#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer/Renderer.h"
#include "hzpch.h"
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
static GLenum ShaderTypeFromString(const std::string &type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || "pixel")
        return GL_FRAGMENT_SHADER;

    HZ_CORE_ASSERT(false, "Unknown Shader Type");

    return GL_NONE;
}

std::string OpenGLShader::ReadFile(std::string_view filePath)
{
    HZ_PROFILE_FUNCTION();

    // std::ios::in       : read only
    // std::ios::binary : read it as binary
    std::ifstream in(filePath.data(), std::ios::in | std::ios::binary);
    std::string result;

    if (in)
    {
        // 파일 포인터를 파일 끝으로 보낸다.
        in.seekg(0, std::ios::end);

        // 파일 크기만큼 resize
        result.resize(in.tellg());

        // 다시 파일 처음으로 돌아가기
        in.seekg(0, std::ios::beg);

        // 파일 정보 읽어들이기
        in.read(&result[0], result.size());
    }
    else
    {
        HZ_CORE_ERROR("Could not open file : '{0}'", filePath);
    }
    return result;
}
std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(
    const std::string &source)
{
    HZ_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> shaderSources;

    // keyword looking for
    const char *typeToken = "#type";

    size_t typeTokenLength = strlen(typeToken);

    // 맨 처음에는 0 이 될 것이다. (만약 glsl 맨 윗줄에 #type 을 적어두었다면)
    size_t pos = source.find(typeToken, 0);

    // 모든 #type token 을 찾을 것이다.
    while (pos != std::string::npos)
    {
        // 위에서 찾은 pos 이후부터 new line 으로 넘어가는 지점을 찾는다.
        size_t eol = source.find_first_of("\r\n", pos);
        HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");

        // read 시작 위치
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        HZ_CORE_ASSERT(ShaderTypeFromString(type),
                       "Invalid shader type specified");

        // read 끝 위치
        size_t nextLinePos = source.find_first_not_of("\r\n", eol);

        // 그 다음 #type 직전 위치까지 찾는다. ex) fragment [이부분부터] ~~ [여기까지]#type
        pos = source.find(typeToken, nextLinePos);

        // 해당 Type 의 Shader 코드를 읽는다.
        shaderSources[ShaderTypeFromString(type)] = source.substr(
            nextLinePos,
            pos - (nextLinePos == std::string::npos ? source.size() - 1
                                                    : nextLinePos));
    }

    return shaderSources;
}
void OpenGLShader::Compile(
    const std::unordered_map<GLenum, std::string> &shaderSources)
{
    HZ_PROFILE_FUNCTION();

    // Vertex and fragment OpenGLShaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    // (참고 : OpenGLShader 라는 소스코드를 컴파일 하면 실행시킬 수 있는 프로그램을 얻는다)
    // GLuint program = glCreateProgram();
    GLuint program = glCreateProgram();

    HZ_CORE_ASSERT(shaderSources.size() <= 2,
                   "There has to be at least 2 shader sources");

    std::array<GLenum, 2> glShaderIDs;

    int glShaderIDIndex = 0;

    for (auto &key : shaderSources)
    {
        GLenum shaderType = key.first;
        const std::string &shaderString = key.second;

        // Create an empty OpenGLShader handle (create OpenGLShader)
        GLuint shader = glCreateShader(shaderType);

        // Send the vertex OpenGLShader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar *sourceCStr = shaderString.c_str();
        glShaderSource(shader, 1, &sourceCStr, 0);

        // Compile the vertex OpenGLShader
        glCompileShader(shader);

        // Check if compliation succeded or failed
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

        // Fail Compile
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;

            // Get Length of logs (error message)
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the OpenGLShader anymore.
            // (어차피 return 해서 나갈 것임)
            glDeleteShader(shader);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "shader compliation failure !");
            return;
        }

        // Attach our OpenGLShaders to our program
        glAttachShader(program, shader);

        glShaderIDs[glShaderIDIndex++] = shader;
    }


    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetOpenGLShader*.
    // Check Linking Failure
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);

        // Don't leak OpenGLShaders either.
        for (auto &shaderID : glShaderIDs)
        {
            glDeleteShader(shaderID);
        }
        HZ_CORE_ERROR("OpenGLShader link failure");
        HZ_CORE_ERROR("{0}", infoLog.data());
        HZ_CORE_ASSERT(false, "!");
        return;
    }

    // Always detach OpenGLShaders after a successful link.
    // Don't leak OpenGLShaders either.
    for (auto &shaderID : glShaderIDs)
    {
        glDetachShader(program, shaderID);
    }

    // 모두 성공할 경우 이때 비로소 m_RendererID 에 값을 세팅한다.
    m_RendererID = program;
}
OpenGLShader::OpenGLShader(std::string_view filePath)
{
    HZ_PROFILE_FUNCTION();

    std::string shaderCode = std::move(ReadFile(filePath));
    auto shaderSource = std::move(PreProcess(shaderCode));
    Compile(shaderSource);

    // assets/shaders/Texture.glsl -> Texture 추출
    auto lastSlash = filePath.find_last_of("/\\");

    // lastSlash + 1 ? : '/' 이후 'T' 에 해당하는 위치로 이동하기 위해 + 1
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

    auto lastDot = filePath.rfind('.');

    // assets/shaders/Texture 와 같이 '.' 가 없으면 "Texture" 부분만 뽑아내야 한다. ==filePath.size() - lastSlash
    // 반면 Texture.glsl 와 같이 '.'가 있으면, '.' 위치 - 'T' 위치를 해줘야 count 가 나온다.
    auto count = lastDot == std::string::npos ? filePath.size() - lastSlash
                                              : lastDot - lastSlash;

    m_Name = std::move(std::string(filePath.substr(lastSlash, count)));
}
OpenGLShader::OpenGLShader(std::string_view name,
                           std::string_view vertexSrc,
                           std::string_view fragmentSrc)
    : m_Name(name)
{
    HZ_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;

    Compile(sources);
}

OpenGLShader::~OpenGLShader()
{
    HZ_PROFILE_FUNCTION();

    glDeleteProgram(m_RendererID);
}
void OpenGLShader::Bind() const
{
    HZ_PROFILE_FUNCTION();

    // 해당 OpenGLShader 로 무언가를 그리고 싶을 때
    // 해당 함수를 먼저 실행할 것이다.
    glUseProgram(m_RendererID);
}
void OpenGLShader::Unbind() const
{
    HZ_PROFILE_FUNCTION();

    glUseProgram(0);
}

void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value)
{
    UploadUniformMat4(name, value);
}

void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value)
{
    UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value)
{
    UploadUniformFloat4(name, value);
}

void OpenGLShader::SetInt(const std::string &name, int value)
{
    UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string &name,
                               int *values,
                               uint32_t count)
{
    UploadUniformIntArray(name, values, count);
}

void OpenGLShader::SetFloat(const std::string &name, float value)
{
    UploadUniformFloat(name, value);
}

void OpenGLShader::UploadUniformBool(const std::string &name, const bool &val)
{
}
void OpenGLShader::UploadUniformInt(const std::string &name, const int &val)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, val);
}
void OpenGLShader::UploadUniformIntArray(const std::string &name,
                                         int *values,
                                         uint32_t count)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1iv(location, count, values);
}
void OpenGLShader::UploadUniformFloat(const std::string &name, const float &val)
{
    // OpenGLShader 에 선언된 uniform 변수를 가져오기
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());

    // uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
    glUniform1f(location, val);
}
void OpenGLShader::UploadUniformFloat2(const std::string &name,
                                       const glm::vec2 &vec)
{
    // OpenGLShader 에 선언된 uniform 변수를 가져오기
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());

    // uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
    glUniform2f(location, vec.x, vec.y);
}
void OpenGLShader::UploadUniformFloat3(const std::string &name,
                                       const glm::vec3 &val)
{
    // OpenGLShader 에 선언된 uniform 변수를 가져오기
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());

    // uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
    glUniform3f(location, val.x, val.y, val.z);
}
void OpenGLShader::UploadUniformFloat4(const std::string &name,
                                       const glm::vec4 &vec)
{
    // OpenGLShader 에 선언된 uniform 변수를 가져오기
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());

    // uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}
void OpenGLShader::UploadUniformMat3(const std::string &name,
                                     const glm::mat3 &mat)
{
    // OpenGLShader 에 선언된 uniform 변수를 가져오기
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());

    // uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}
void OpenGLShader::UploadUniformMat4(const std::string &name,
                                     const glm::mat4 &mat)
{
    // OpenGLShader 에 선언된 uniform 변수를 가져오기
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());

    // uniform 변수를 OpenGLShader 쪽에 세팅해주는 함수
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

} // namespace Hazel