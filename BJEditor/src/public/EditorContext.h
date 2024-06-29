#pragma once
#include "Editor.h"

namespace HazelEditor
{

class EditorContext
{
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
    
        /*
         * Log 폴더
		 */
        static const char *log;
    
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
    
        /**builtinShader 폴더
		 */
        // static const char *builtinShader;
    
        /*
         * builtin 폴더
		 */
        // static const char *builtin;
    };

    static Editor* Initialize();

    EditorContext();
    ~EditorContext();

private:
    static EditorContext* m_Instance;
};
};

