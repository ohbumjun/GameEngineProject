#include "hzpch.h"
#include "Hazel/Core/Reflection/TypeUtils.h"

std::size_t TypeUtils::wrapped_type_name_prefix_length()
{
	// 참고 : constexpr 키워드가 붙은 함수 안에서 std::cout 과 같은 runtime func 를 호출하려고 하면 compile error
	// std::cout << "TypeName<type_name_prober>() : " << TypeName<type_name_prober>() << std::endl;
	// std::cout << "wrapped_type_name<type_name_prober>() : " << wrapped_type_name<type_name_prober>() << std::endl;


	// 1) wrapped_type_name<type_name_prober>() ?
	// -> wrapped_type_name<type_name_prober>() : class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Reflection::detail::wrapped_type_name<void>(void)
	// 2) TypeName<type_name_prober>() == "void" : 위의 void template 특수화 함수를 호출한다.
	// 해당 string 에서 TypeName<type_name_prober>() 의 첫번째 index 를 리턴한다
	// 3) 결과값 : TypeName<type_name_prober>() 는 void 이므로, 맨 끝에서 2번째 void 의 시작 위치를 리턴하게 된다.

	static const std::string voidTypeName = "void";
	return wrapped_type_name<void>().find(voidTypeName);
}

std::size_t TypeUtils::wrapped_type_name_suffix_length()
{
	static const std::string voidTypeName = "void";

	// ex. 123 (총 길이) - 112(void 맨 처음 시작위치) - 4 (void 길이)
	// "(void)" 만큼의 길이 
	return wrapped_type_name<void>().length()
		- wrapped_type_name_prefix_length()
		- voidTypeName.length();
}
