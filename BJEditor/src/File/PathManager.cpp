#include "PathManager.h"
#include "Hazel/FileSystem/DirectorySystem.h"

HazelEditor::PathManager* HazelEditor::PathManager::m_SingleInst = nullptr;
std::unordered_map<uint8, HazelEditor::PathStruct*> HazelEditor::PathManager::m_AssetPaths{};
std::unordered_map<uint8, std::string> HazelEditor::PathManager::m_AssetExt{};

namespace HazelEditor
{
	void PathManager::initialize()
	{
		addRootPath();

		// m_AssetPaths[(uint8)AssetPathEnum::Scene] = "assets/scene";
		// m_AssetPaths[(uint8)AssetPathEnum::Scene] = "assets/scene";
		// m_AssetPaths[(uint8)AssetPathEnum::Game] = "assets/game";
		// m_AssetPaths[(uint8)AssetPathEnum::Texture] = "assets/textures";
		// m_AssetPaths[(uint8)AssetPathEnum::Shader] = "assets/shaders";
		// m_AssetPaths[(uint8)AssetPathEnum::Font] = "assets/fonts";

		m_AssetExt[(uint8)AssetExt::Scene] = ".scene";
		// m_AssetPaths[(uint8)AssetPathEnum::Game] = "assets/game";
		// m_AssetPaths[(uint8)AssetPathEnum::Texture] = "assets/textures";
		m_AssetExt[(uint8)AssetExt::Shader] = ".shader";
		// m_AssetPaths[(uint8)AssetPathEnum::Font] = "assets/fonts";
	}

	void PathManager::addAssetPath(AssetPathEnum pathEnum)
	{
		if (GetAssetPath(pathEnum))
		{
			return;
		}

	}

	void PathManager::addRootPath()
	{
		TCHAR   RootPath[MAX_PATH] = {};

		// GetModuleFileName : 현재 실행파일이 있는 경로 + 실행파일이름.exe 로
		// 전체경로를 얻어오게 된다.
		GetModuleFileName(0, RootPath, MAX_PATH);

		// 얻어온 문자열을 뒤에서부터 차례로 접근하여 \나 / 가 있는 문자를 찾는다.
		int Length = lstrlen(RootPath);

		// D:\Lecture\37th\API\GameFramework\GameFramework\Bin\GameFramework_Debug.exe
		for (int i = Length - 1; i > 0; --i)
		{
			if (RootPath[i] == '\\')
			{
				// 이후 경로는 모두 초기화
				memset(&RootPath[i + 1], 0, sizeof(TCHAR) * (Length - i - 1));
				break;
			}
		}

		PathStruct* pathStruct = new PathStruct;

		lstrcpy(pathStruct->Path, RootPath);

#ifdef UNICODE
		// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
		DirectorySystem::FromSystemToUTF8Path(pathStruct->PathMultibyte, RootPath);
		// int ConvertLength = WideCharToMultiByte(CP_ACP, 0, BinPath, -1, nullptr, 0, 0, 0);
		// 
		// WideCharToMultiByte(CP_ACP, 0, BinPath, -1,
		// 	Info->PathMultibyte, ConvertLength, 0, 0);
#else
		// strcpy_s(Info->PathMultibyte, BinPath);
#endif // UNICODE

		m_AssetPaths.insert({(uint8)AssetPathEnum::Root, pathStruct });
	}

	const PathStruct* PathManager::GetAssetPath(AssetPathEnum pathEnum)
	{
		if (m_AssetPaths.count((uint8)pathEnum) == 0)
		{
			return nullptr;
		}

		return m_AssetPaths[(uint8)pathEnum];
	}

	const std::string& PathManager::GetAssetExt(AssetExt pathEnum)
	{
		return m_AssetExt[(uint8)pathEnum];
	}
}
