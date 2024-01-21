#pragma once

#include "hzpch.h"


namespace HazelEditor
{
	enum class AssetPathEnum
	{
		Root,
		Scene,
		Game,
		Texture,
		Shader,
		Font
	};

	enum class AssetExt
	{
		Scene,
		Shader,
	}; 
	
	struct PathStruct
	{
		// TCHAR : 유니코드 문자열을 저장하기 위한 타입이다.
		// 단, 프로젝트가 Multibyte 문자열을 사용한다면 char 타입으로 정의된다.
		// 유니코드일 경우 wchar_t 로 정의된다.
		TCHAR	Path[MAX_PATH];
		char		PathMultibyte[MAX_PATH];

		PathStruct() :
			Path{},
			PathMultibyte{}
		{
		}
	};

	class PathManager
	{
		friend class FileManager;
	public:
		static PathManager* GetPathInfo()
		{
			if (m_SingleInst == nullptr)
			{
				m_SingleInst = new PathManager();
			}

			return m_SingleInst;
		}
		static void DestroyInst()
		{
			if (m_SingleInst) delete m_SingleInst;

			auto    iter = m_AssetPaths.begin();
			auto    iterEnd = m_AssetPaths.end();

			for (; iter != iterEnd; ++iter)
			{
				if (iter->second)
				{
					delete iter->second;
				}
			}
		}

		static const PathStruct* GetAssetPath(AssetPathEnum pathEnum);
		static const std::string& GetAssetExt(AssetExt pathEnum);
	private:
		static void initialize();
		static void addAssetPath(AssetPathEnum pathEnum);
		static void addRootPath();
		static PathManager* m_SingleInst;
		static std::unordered_map<uint8, PathStruct*> m_AssetPaths;
		static std::unordered_map<uint8, std::string> m_AssetExt;
	};
}


