#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel
{
	/*
	Adapter pattern is about making
	2 interfaces that arent't compatible
	compatible
	*/
	class AssetManagerController
	{
    public:
        static void Initialize(AssetManagerController *assetManager);
        static void Finalize()
        {
            if (s_Instance)
            {
                delete s_Instance;
            }
        }
        static void LoadAsset(const std::string &relativePath);
        static AssetManagerController *GetInst()
        {
            return s_Instance;
        }

    protected:
        virtual void loadAsset(const std::string &relativePath) = 0;
    private:
        static AssetManagerController *s_Instance;
	};

	class BundlePackManagerController : public AssetManagerController
	{

	};
} // namespace Hazel
