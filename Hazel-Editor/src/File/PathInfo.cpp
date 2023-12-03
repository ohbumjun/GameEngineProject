#include "PathInfo.h"

PathInfo* PathInfo::m_SingleInst = nullptr;
std::unordered_map<uint8, std::string> PathInfo::m_AssetPaths{};

void PathInfo::initializeAssetPath()
{
	m_AssetPaths[(uint8)AssetPathEnum::Scene]	= "assets/scene";
	m_AssetPaths[(uint8)AssetPathEnum::Game]		= "assets/game";
	m_AssetPaths[(uint8)AssetPathEnum::Texture]	= "assets/textures";
	m_AssetPaths[(uint8)AssetPathEnum::Shader]	= "assets/shaders";
	m_AssetPaths[(uint8)AssetPathEnum::Font]		= "assets/fonts";
}

const std::string& PathInfo::GetAssetPath(AssetPathEnum pathEnum)
{
	return m_AssetPaths[(uint8)pathEnum];
}