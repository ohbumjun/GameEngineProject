#pragma once

#include "hzpch.h"

enum class AssetPathEnum 
{
	Scene,
	Game,
	Texture,
	Shader,
	Font
};

class PathInfo
{
	friend class FileManager;
public:
	static PathInfo* GetPathInfo()
	{
		if (m_SingleInst == nullptr)
		{
			m_SingleInst = new PathInfo();
		}

		return m_SingleInst;
	}
	static void DestroyInst()
	{
		if (m_SingleInst) delete m_SingleInst;
	}

	static const std::string& GetAssetPath(AssetPathEnum pathEnum);
private :
	static void initializeAssetPath();
	static PathInfo* m_SingleInst;
	static std::unordered_map<uint8, std::string> m_AssetPaths;
};