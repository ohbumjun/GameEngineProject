#pragma once

// remove_all_pointers 라는 구조체는 member "Type" 에 template 인자로 들어온 "T" type 을 세팅한다. 
template <typename T>
struct remove_all_pointers
{
	using Type = T;
};

// T* 타입에 대한 템플릿 특수화 이다. 마찬가지로, 템플릿 인자로 포인터 타입이 들어오더라도
// 이에 대한 remove_all_pointers 구조체는 member "Type" 에 포인터를 제거한 원본 T Type 을 세팅한다. 
template <typename T>
struct remove_all_pointers<T*>
{
	using Type = typename remove_all_pointers<T>::Type;
};

// 최종적으로 remove_all_pointers_t 는 remove_all_pointers 구조체의 member "Type". 즉, 인자로 들어온 Type 그대로
// 를 가리키도록 세팅한다. 
template <typename T>
using remove_all_pointers_t = typename remove_all_pointers<T>::Type;

template <typename T>
struct IsArray {
	static constexpr bool value = false;
};

template <typename T, std::size_t N>
struct IsArray<std::array<T, N>> {
	static constexpr bool value = true;
};

template <typename T, std::size_t N>
struct IsArray<T[N]> {
	static constexpr bool value = true;
};

template <typename T, typename Alloc>
struct IsArray<std::vector<T, Alloc>> {
	static constexpr bool value = true;
};

// 우선 std::set, std::list, unorderd_set, map, unordered_map 을 Iterable 로 간주한다.
template <typename T>
struct IsIterable {
	static constexpr bool value = false;
};

template <typename T, typename Alloc>
struct IsIterable<std::list<T, Alloc>> {
	static constexpr bool value = true;
};

template <typename T, typename Compare, typename Alloc>
struct IsIterable<std::set<T, Compare, Alloc>> {
	static constexpr bool value = true;
};

template <typename T, typename Hash, typename KeyEqual, typename Alloc>
struct IsIterable<std::unordered_set<T, Hash, KeyEqual, Alloc>> {
	static constexpr bool value = true;
};

template <typename Key, typename T, typename Compare, typename Alloc>
struct IsIterable<std::map<Key, T, Compare, Alloc>> {
	static constexpr bool value = true;
};

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
struct IsIterable<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> {
	static constexpr bool value = true;
};

class TypeUtils
{
public:
	// 현재는 Reflection.h에서만 해당 함수들을 사용중이지만 
	// 이 또한 멀티쓰레드 환경에서 동기화가 필요해보인다.
	template <typename T>
	static std::string wrapped_type_name();
	static std::size_t wrapped_type_name_prefix_length();
	static std::size_t wrapped_type_name_suffix_length();

	template<typename T>
	static constexpr uint32_t CountPointers(uint32_t counter = 0);

	template <typename T>
	static void CheckArray(const T& member);

	template <typename T>
	static void CheckIterable(const T& member);
};

template<typename T>
inline std::string TypeUtils::wrapped_type_name()
{
#ifdef __clang__
	return __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
	return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
	return __FUNCSIG__;
#else
#error "Unsupported compiler"
#endif
}

template<typename T>
inline constexpr uint32_t TypeUtils::CountPointers(uint32_t counter)
{
	if constexpr (std::is_pointer_v<T>)
		return CountPointers<std::remove_pointer_t<T>>(++counter);
	else
		return counter;
}

template <typename T>
inline void TypeUtils::CheckArray(const T& member) {
	if (IsArray<T>::value) return true;
	else return false;
}

template <typename T>
inline void TypeUtils::CheckIterable(const T& member) {
	if (IsIterable<T>::value) return true;
	else return false;
}



