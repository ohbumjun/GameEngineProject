#pragma once

namespace Hazel
{
	struct DefaultHeapAllocator
	{
		void* Allocate(size_t size) ;

		template<typename T>
		void* Allocate() { return Allocate(sizeof(T)); }

		// template<typename T, typename... TArgs>
		// T* New(const char* file, const size_t line, TArgs... args)
		// {
		// 	return new(Alloc<T>(file, line))T(args...);
		// }

		void* Reallocate(void* ptr, size_t size) ;

		template<typename T>
		void* Reallocate(void* ptr) { return Realloc(ptr, sizeof(T), file, line); }

		void Free(void* ptr) ;
	};

}
