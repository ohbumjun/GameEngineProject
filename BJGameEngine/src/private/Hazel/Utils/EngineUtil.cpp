#include "hzpch.h"
#include "EngineUtil.h"

namespace Utils
{

	const size_t EngineUtil::CalculatePadding(const size_t baseAddress, const size_t alignment)
	{
		const size_t multiplier = (baseAddress / alignment) + 1;
		const size_t alignedAddress = (multiplier * alignment);
		const size_t padding = alignedAddress - baseAddress;
		return padding;
	}

	const size_t EngineUtil::CalculatePaddingWithHeader(const size_t baseAddress, const size_t alignment, const size_t HeaderSize)
	{
		size_t padding = CalculatePadding(baseAddress, alignment);

		size_t neededSpace = HeaderSize;

		if (neededSpace > padding)
		{
			neededSpace = neededSpace - padding;

			if (neededSpace % alignment > 0)
				padding += alignment * (1 + (neededSpace / alignment));
			else
				padding += alignment * (neededSpace / alignment);
		}

		return padding;
	}

}