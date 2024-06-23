#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel
{
	// Engine, Editor ������ Asset ���� Manager ���� Class
	class AssetManagerBase
	{
    public:
        static void Initialize(AssetManagerBase *assetManager)
		{
            HZ_CORE_ASSERT(assetManager, "AssetManager is nullptr");
            HZ_CORE_ASSERT(s_Instance == nullptr, "AssetManager Already Initialized");
		}
		static void Finalize()
		{
			if (s_Instance)
			{
                delete s_Instance;
			}
		}
    private:
        static AssetManagerBase *s_Instance;
	};
} // namespace Hazel
