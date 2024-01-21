#pragma once

namespace Utils
{
	class EngineUtil
	{
	public:
		static const size_t CalculatePadding(const size_t baseAddress,
			const size_t alignment);

		static const size_t CalculatePaddingWithHeader(const size_t baseAddress,
			const size_t alignment, const size_t HeaderSize);
	};
}

