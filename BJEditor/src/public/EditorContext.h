#pragma once
#include "Editor.h"
#include "EditorSettings.h"

namespace HazelEditor
{

class EditorContext
{
    friend class ProjectContext;

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
        
		struct Cache
        {
		    // @brief ���� ���� ���ڿ� ("Assets")
            static const char *asset;

			// @brief ���÷�Ʈ ���� ���ڿ� ("Reflect")
            static const char *reflect;

			// @brief ���̴� ���� ���ڿ� ("Shaders")
            static const char *shader;

			// @brief built-in shader ���� ���ڿ�("BuiltInShader")
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

