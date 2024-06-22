#include "EditorContext.h"
#include <string>
#include "FileSystem.h"

static const std::string& stringBuiltIn = FileSystem::CombinePath("BuiltIn", "Resource");

const char* EditorContext::Directories::engine = "Engine";
const char* EditorContext::Directories::bundle = "Bundle";
const char* EditorContext::Directories::settings = "Settings";
const char* EditorContext::Directories::solution = "Solution";
const char* EditorContext::Directories::library = "Library";
const char* EditorContext::Directories::log = "Log";
const char* EditorContext::Directories::project = "Project";
const char* EditorContext::Directories::resources = "Resources";
const char* EditorContext::Directories::builtin = "BuiltIn";
const char* EditorContext::Directories::builtinresource = stringBuiltIn.c_str();
//const char* EditorContext::Directories::builtinShader = stringBuiltinShader.c_str();
const char* EditorContext::Directories::program = "Program";
