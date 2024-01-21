#pragma once

#include "Hazel/Asset/Image/Texture.h"
#include <filesystem>

namespace HazelEditor {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
		Hazel::Ref<Hazel::Texture2D> m_DirectoryIcon;
		Hazel::Ref<Hazel::Texture2D> m_FileIcon;
	};

}