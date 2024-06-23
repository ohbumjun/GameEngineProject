#pragma once

namespace HazelEditor
{

class EditorContext
{
public:
    // 에디터 사용 폴더명 모음.
    struct Directories
    {
        /**솔루션 폴더 문자열 ("Solution")
		 */
        static const char *solution;

        /**엔진 폴더 문자열 ("Engine")
		 */
        static const char *engine;

        /**솔루션 폴더 문자열 ("Project")
		 */
        static const char *project;

        /**셋팅 폴더 문자열 ("Settings")
		 */
        static const char *settings;

        /**솔루션 폴더 문자열 ("Resources")
		 */
        static const char *resources;

        /**엔진 폴더 문자열 ("Engine")
		 */
        static const char *bundle;

        /**builtin/Resource 폴더
		 */
        static const char *builtinresource;

        /**소스 폴더 문자열 ("Program")
		 */
        static const char *program;

        /**library 폴더
		 */
        static const char *library;

        /**builtinShader 폴더
		 */
        static const char *builtinShader;

        /**Log 폴더
		 */
        static const char *log;

        /**builtin 폴더
		 */
        // static const char *builtin;
    };
};
};

