#pragma once

#include "Hazel/Resource/Shader/Shader.h"

#include <glm/glm.hpp>

// TODO Remove
typedef unsigned int GLenum;

namespace Hazel
{
class OpenGLShader : public Shader
{
public:
    OpenGLShader(std::string_view name,
                 std::string_view vertexSrc,
                 std::string_view fragmentSrc);
    OpenGLShader(std::string_view filePath);
    virtual ~OpenGLShader();

    void Bind() const override;
    void Unbind() const override;

    virtual void SetMat4(const std::string &name,
                         const glm::mat4 &value) override;
    virtual void SetFloat3(const std::string &name,
                           const glm::vec3 &value) override;
    virtual void SetFloat4(const std::string &name,
                           const glm::vec4 &value) override;
    virtual void SetInt(const std::string &name, int value) override;
    virtual void SetIntArray(const std::string &name,
                             int *values,
                             uint32_t count) override;
    virtual void SetFloat(const std::string &name, float value);

    virtual const std::string &GetName() const override
    {
        return m_Name;
    }

    void UploadUniformBool(const std::string &name, const bool &val);

    void UploadUniformInt(const std::string &name, const int &val);
    void UploadUniformIntArray(const std::string &name,
                               int *values,
                               uint32_t count);

    void UploadUniformFloat(const std::string &name, const float &val);
    void UploadUniformFloat2(const std::string &name, const glm::vec2 &val);
    void UploadUniformFloat3(const std::string &name, const glm::vec3 &val);
    void UploadUniformFloat4(const std::string &name, const glm::vec4 &val);

    void UploadUniformMat3(const std::string &name, const glm::mat3 &mat);
    void UploadUniformMat4(const std::string &name, const glm::mat4 &mat);

private:
    std::string ReadFile(std::string_view filePath);

    // Type of Shader ~ Shader Source Code
    std::unordered_map<GLenum, std::string> PreProcess(
        const std::string &source);
    void Compile(const std::unordered_map<GLenum, std::string> &shaderSources);

private:
    // 현재 해당 객체를 식벽하는 ID
    uint32_t m_RendererID;
    std::string m_Name;
};
} // namespace Hazel
