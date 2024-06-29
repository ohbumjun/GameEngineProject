#pragma once
#include "Editor.h"

namespace HazelEditor
{

class EditorContext
{
public:
    // ������ ��� ������ ����.
    struct Directories
    {
        /**���� ���� ���ڿ� ("Engine")
		 */
        static const char *engine;
    
        /**���� ���� ���ڿ� ("Engine")
		 */
        static const char *bundle;
    
        /**���� ���� ���ڿ� ("Settings")
		 */
        static const char *settings;
    
        /**�ַ�� ���� ���ڿ� ("Solution")
		 */
        static const char *solution;
    
        /**library ����
		 */
        static const char *library;
    
        /*
         * Log ����
		 */
        static const char *log;
    
        /**�ַ�� ���� ���ڿ� ("Project")
		 */
        static const char *project;
    
        /**�ַ�� ���� ���ڿ� ("Resources")
		 */
        static const char *resources;
        
        /**builtin/Resource ����
		 */
        static const char *builtin;
    
        /**�ҽ� ���� ���ڿ� ("Program")
		 */
        static const char *program;
    
        /**builtinShader ����
		 */
        // static const char *builtinShader;
    
        /*
         * builtin ����
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

