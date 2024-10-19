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
        static const char *engine;
    
        static const char *settings;
    
        static const char *solution;
    
        static const char *library;
    
        static const char *project;

        static const char *resources;
        
        static const char *program;
        
		struct Cache
        {
            static const char *asset;

            static const char *reflect;

            static const char *shader;
        };
    };

    static Editor* Initialize();

    EditorContext();
    ~EditorContext();

    const EditorSettings& GetSettings() const { return m_Settings; }

    void LoadSettings();
    void SaveSettings();

private:
    class Project* m_Project;
    EditorSettings m_Settings;
    std::string _settingsFilePath;
};
EditorContext* BJ_GetEditorContext();
const char *BJ_GetEditorDirRootPath();
};

