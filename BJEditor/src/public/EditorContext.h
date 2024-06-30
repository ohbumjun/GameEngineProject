#pragma once
#include "Editor.h"
#include "EditorSettings.h"

namespace HazelEditor
{

class EditorContext
{
    friend class ProjectContext;

public:
    // 에디터 사용 폴더명 모음.
    struct Directories
    {
        /**엔진 폴더 문자열 ("Engine")
		 */
        static const char *engine;
    
        /**엔진 폴더 문자열 ("Engine")
		 */
        static const char *bundle;
    
        /**셋팅 폴더 문자열 ("Settings")
		 */
        static const char *settings;
    
        /**솔루션 폴더 문자열 ("Solution")
		 */
        static const char *solution;
    
        /**library 폴더
		 */
        static const char *library;
    
        /**솔루션 폴더 문자열 ("Project")
		 */
        static const char *project;
    
        /**솔루션 폴더 문자열 ("Resources")
		 */
        static const char *resources;
        
        /**builtin/Resource 폴더
		 */
        static const char *builtin;
    
        /**소스 폴더 문자열 ("Program")
		 */
        static const char *program;
        
		struct Cache
        {
		    // @brief 에셋 폴더 문자열 ("Assets")
            static const char *asset;

			// @brief 리플렉트 폴더 문자열 ("Reflect")
            static const char *reflect;

			// @brief 셰이더 폴더 문자열 ("Shaders")
            static const char *shader;

			// @brief built-in shader 폴더 문자열("BuiltInShader")
            static const char *builtInShader;
        };
    };

    static Editor* Initialize();

    EditorContext();
    ~EditorContext();
    void LoadSettings();
    void SaveSettings();

private:
    static EditorContext* m_Instance;
    class Project* m_Project;
    EditorSettings m_Settings;
};

EditorContext* BJ_GetEditorContext();
};

