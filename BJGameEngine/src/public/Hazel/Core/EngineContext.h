#pragma once

namespace Hazel
{

class EngineContext
{
public:
    // Engine 사용 폴더명 모음.
    struct Directories
    {
        /*
        * builtin 폴더
		 */
        static const char *defaultAssets;
    };

    struct ResourceDirectories
    {
        static const char *fonts;
        static const char *shaders;
        static const char *textures;
    };

    static void Initialize();
    static std::string GetDefaultAssetPath()
    {
        return m_DefaultAssetsPath;
    }
    // static std::string GetDefaultResourcePath()
    // {
    //     return m_DefaultResourcePath;
    // }
    static std::string GetResourceRootPath()
    {
        return m_ResourceRootPath;
    }
 private:
    static std::string m_DefaultAssetsPath;
    // static std::string m_DefaultResourcePath;
    static std::string m_ResourceRootPath;
};
}; // namespace HazelEditor
