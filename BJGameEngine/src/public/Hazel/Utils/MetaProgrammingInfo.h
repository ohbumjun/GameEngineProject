#pragma once

#include "hzpch.h"

// Reference : https://github.com/electronicarts/EASTL/blob/master/include/EASTL/bonus/call_traits.h

template <typename T, bool isSmall>
struct call_type_impl2
{
    typedef const T &ParamType;
};

// isSmall 가 true 인 것에 대한 템플릿 특수화
template <typename T>
struct call_type_impl2<T, true>
{
    typedef const T ParamType;
};

/*
(사용 예시)
call_type_impl2<int, false>::ParamType x = 42;  // x is a const int&
call_type_impl2<int, true>::ParamType y = 42;   // y is a const int
std::cout << "x: " << x << std::endl;
std::cout << "y: " << y << std::endl;
*/

template <typename T, bool isPointer, bool b1>
struct call_type_impll
{
    typedef const T &ParamType;
};

// T 가 포인터								: const T
// T 가 포인터가 아님 && 4 byte 보다 크다	: const T&
// 참고 : sizeof(void*) = 32 bit ? 4 byte / 64 bit ? 8 byte;
template <typename T, bool isPointer>
struct call_type_impll<T, isPointer, true>
{
    typedef typename call_type_impl2<T, sizeof(T) <= sizeof(void *)>::ParamType
        ParamType;
};

template <typename T, bool b1>
struct call_type_impll<T, true, b1>
{
    typedef T const ParamType;
};

template <typename T>
struct call_traits
{
public:
    typedef T ValueType;
    typedef T &Reference;
    typedef const T &ConstReference;
    // std::is_arithmetic<T>::value : arithmethic type 에 해당하면 true / false
    // ex) int, float, double, char, bool

    // ex) o pointer				-> call_type_impll<T, true, b1>			-> int*			? == const int*
    // ex) x pointer (int)			-> call_type_impll<T, isPointer, true>	-> int			? == call_type_impl2<T, true> == const int
    // ex) x pointer (Component)	-> call_type_impll<T, isPointer, false>	-> Component	? == const& Component
    typedef typename call_type_impll<T,
                                     std::is_pointer<T>::value,
                                     std::is_arithmetic<T>::value>::ParamType
        ParamType;

    static const int value = 0;
};

#pragma region>> call_traits example


// template <typename T>
// void print_traits_info() {
// 	using Traits = call_traits<T>;
//
// 	std::cout << "Type: " << typeid(T).name() << std::endl;
// 	std::cout << "ValueType: " << typeid(typename Traits::ValueType).name() << std::endl;
// 	std::cout << "Reference: " << typeid(typename Traits::Reference).name() << std::endl;
// 	std::cout << "ConstReference: " << typeid(typename Traits::ConstReference).name() << std::endl;
// 	std::cout << "ParamType: " << typeid(typename Traits::ParamType).name() << std::endl;
// 	std::cout << "Value: " << Traits::value << std::endl;
// 	std::cout << "-----------------------------------\n";
// }
//
// int main() {
// 	int i = 42;
// 	const double pi = 3.14159;
// 	int& ri = i;
//
// 	print_traits_info<int>();            // Using primary template
// 	print_traits_info<const double>();   // Using primary template
// 	print_traits_info<int*>();           // Using primary template
// 	print_traits_info<int&>();           // Using specialization for references
//
// 	/*
// 	Type: int
// 	ValueType: int
// 	Reference: int
// 	ConstReference: int
// 	ParamType: int
// 	Value: 0
// 	-----------------------------------
// 	Type: double
// 	ValueType: double
// 	Reference: double
// 	ConstReference: double
// 	ParamType: double
// 	Value: 0
// 	-----------------------------------
// 	Type: int * __ptr64
// 	ValueType: int * __ptr64
// 	Reference: int * __ptr64
// 	ConstReference: int * __ptr64
// 	ParamType: int * __ptr64
// 	Value: 0
// 	-----------------------------------
// 	Type: int
// 	ValueType: int
// 	Reference: int
// 	ConstReference: int
// 	ParamType: int
// 	Value: 1
// 	-----------------------------------
// 	*/
//
// 	return 0;
// }

#pragma endregion

// call_traits 에 대한 & type 특수화
template <typename T>
struct call_traits<T &>
{
public:
    typedef T &ValueType;
    typedef T &Reference;
    typedef const T &ConstReference;
    typedef T &ParamType;

    static const int value = 1;
};


template <typename T, typename = void>
struct is_callable : std::is_function<T>
{
};

// - (std::true_type) 을 상속받고
// - T class 의 operator() 멤버에 대한 특수화 이다.
// T 가 operator () 가 존재하는지를 검사한다.
// decltype(void(&T::operator())) 는 operator () 의 리턴타입을 조사한다.
// void(&T::operator()) 은 operator () 멤버 함수의, 주소를 가져오기 위한 표현이고
// decltype 을 붙임으로써 리턴타입을 확인할 수 있다.
// 만약 operator () 가 존재하고, void 리턴타입이라면, T 는 함수처럼 사용될 수 있다.
template <typename T>
struct is_callable<
    T,
    typename std::enable_if<
        std::is_same<decltype(void(&T::operator())), void>::value>::type>
    : std::true_type
{
};

#pragma region>> is_base_of_any example

// template <typename T>
// void check_base_class() {
// 	if constexpr (is_base_of_any<T>::value) {
// 		std::cout << "T is a base class of at least one type." << std::endl;
// 	}
// 	else {
// 		std::cout << "T is not a base class of any type." << std::endl;
// 	}
// }
//
// struct Base {};
// struct Derived1 : Base {};
// struct Derived2 : Base {};
// struct NotDerived {};
// int main() {
// 	check_base_class<Base>();      // T is Base, no other types provided, defaults to primary template
// 	check_base_class<Derived>();   // T is Derived, checks if it's a base class of any types
// 	std::cout << std::boolalpha;
//  std::cout << "Base is a base class of Derived1: " << is_base_of_any<Base, Derived1>::value << std::endl;
//  std::cout << "Base is a base class of Derived2: " << is_base_of_any<Base, Derived2>::value << std::endl;
//  std::cout << "Base is a base class of NotDerived: " << is_base_of_any<Base, NotDerived>::value << std::endl;
// 	return 0;
// }

#pragma endregion


// - 가변인자 템플릿의 끝을 의미하는 표현이다.
//   즉, T 이후 더이상의 template 인자가 주어지지 않는다면
//   Primary template 이 기본값으로 사용된다는 것이다.
// template <typename T, typename ...> struct is_base_of_any : std::true_type {};
template <typename T, typename...>
struct is_base_of_any : std::false_type
{
};

// - is_base_of_any 에 대한 가변인자 템플릿
// - std::is_base_of<T, Head>::value : T 가 Head 의 Base Class 인지 확인한다. (Head 가 T 상속)
// - std::integral_constant<bool, std::is_base_of<T, Head>::value ?
//		컴파일 타임 때 std::integral_constant<bool, Value> 에 대한 compile-time constant type 을 만들기
//		위한 것이다.
//   즉, std::is_base_of<T, Head>::value 가
//   1) true 라면 -> std::integral_constant<bool, std::is_base_of<T, Head>::value 는 std::true_type
//   2) false라면 -> std::false_type

// - is_base_of_any<T, Rest...>::value 는 T 가 나머지 template 인자 타입들의 Base Class 인지를
//   재귀적으로 검사하는 로직이다.
//   즉, Rest ... 에 있는 type 중에서 하나라도, T 가 base class 라면, 즉 t 의 파생클래스라면
//   true_type 이 된다.

//  결론 : T 를 제외한 템플릿 클래스 중에서 T 의 파생 클래스가 하나라도 존재하는지를 검사하는 코드
template <typename T, typename Head, typename... Rest>
struct is_base_of_any<T, Head, Rest...>
    : std::integral_constant<bool,
                             std::is_base_of<T, Head>::value ||
                                 is_base_of_any<T, Rest...>::value>
{
};

template <typename C, typename = void>
struct is_iterable : std::false_type
{
};

// - std::is_same<decltype(std::declval<C>().begin()), void>::value :
//   => 1) C Class 가 begin() 멤버 함수가 존재하고
//   => 2) void 를 리턴한다면 true. 그외에는 false

// - !std::is_same<decltype(std::declval<C>().begin()), void>::value &&
//   !std::is_same<decltype(std::declval<C>().end()), void>::value
//   => begin(), end() 함수 둘다 존재하되, 둘다 void 가 아닌 타입을 리턴한다면 true
template <typename C>
struct is_iterable<
    C,
    typename std::enable_if<
        !std::is_same<decltype(std::declval<C>().begin()), void>::value &&
        !std::is_same<decltype(std::declval<C>().end()), void>::value>::type>
    : std::true_type
{
};

template <typename T>
struct remove_all
{ // remove reference
    // remove_all_extents : array 관련 요소 제거 ex) int[3][4] -> int
    using type = typename std::remove_all_extents<
        typename std::remove_const<typename std::remove_reference<
            typename std::remove_pointer<T>::type>::type>::type>::type;
};

template <typename C, bool IsIterable>
struct element_type_impl
{
    using type = void;
};

template <typename C>
struct element_type_impl<C, true>
{
    using type = decltype(std::declval<C>().begin());
};

template <typename C>
struct element_type_impl<C, false>
{
    using type = void;
};

template <typename C>
struct element_type
{
    using type = typename element_type_impl<C, is_iterable<C>::value>::type;
};

#pragma region>> get_index

// int main() {
// 	using TypeList = std::tuple<int, double, char, float, int>;
//
// 	constexpr size_t index1 = get_index<int, int, double, char, float, int>::value;
// 	constexpr size_t index2 = get_index<double, int, double, char, float, int>::value;
// 	constexpr size_t index3 = get_index<char, int, double, char, float, int>::value;
// 	constexpr size_t index4 = get_index<float, int, double, char, float, int>::value;
// 	// 컴파일 에러가 난다.
// 	constexpr size_t index5 = get_index<int, TypeList>::value;
//
// 	std::cout << "Index of int: " << index1 << std::endl;     // Index of int: 0
// 	std::cout << "Index of double: " << index2 << std::endl;  // Index of double: 1
// 	std::cout << "Index of char: " << index3 << std::endl;    // Index of char: 2
// 	std::cout << "Index of float: " << index4 << std::endl;   // Index of float: 3
// 	std::cout << "Index of int in TypeList: " << index5 << std::endl;  // Index of int in TypeList: 0
//
// 	return 0;
// }
#pragma endregion

// structure 를 정의하는 전방선언일 뿐, 해당 struct 에 대한 어떤 정의도 제공하지 않는다.
template <typename T, typename... Ts>
struct get_index;

// - 자기 자신에 대한 특수화. 가변인자 템플릿 형태에서 T,T,... 이렇게 같은 template 이
//	 2번 연속 들어올 경우에 해당한다.
// - std::integral_constant<std::size_t, 0> {};
//   std::size_t 타입의 컴파일 타임 constant 를 표현하기 위한 문법으로, 이 경우 0 을 나타낸다.
//   즉, T 타입이 type 들의 list 들 중에서 0번째 idx 에 위치한다라는 것을 의미한다 .
template <typename T, typename... Ts>
struct get_index<T, T, Ts...> : std::integral_constant<std::size_t, 0>
{
};

// T 이후, T 를 찾아서 계속해서 재귀적으로 get_index 를 찾아가는 것이다.
template <typename T, typename Tail, typename... Ts>
struct get_index<T, Tail, Ts...>
    : std::integral_constant<std::size_t, 1 + get_index<T, Ts...>::value>
{
};

// Type list 가 오직 T 하나 혹은, empty 일 경우에 대한 특수화 이다.
template <typename T>
struct get_index<T>
{
    // 여기에 걸리면, 항상 컴파일 타임 에러를 내뱉을 것이다.
    // condition is always false, but should be dependant of T
    static_assert(sizeof(T) == 0, "element not found");
};

// refenece : https://functionalcpp.wordpress.com/2013/08/05/function-traits/

#pragma region>> function_traits basic example

// int add(int a, int b) {
// 	return a + b;
// }
//
// // Example callable object (function-like object)
// struct Multiply {
// 	int operator()(int a, int b) {
// 		return a * b;
// 	}
// };
//
// int main() {
// 	// Extract information about the add function
// 	using add_traits = function_traits<decltype(add)>;
// 	std::cout << "Return type of add function: " << typeid(add_traits::return_type).name() << std::endl;
// 	std::cout << "Number of arguments for add function: " << add_traits::arg_count << std::endl;
// 	std::cout << "Type of the first argument for add function: " << typeid(add_traits::argument<0>::type).name() << std::endl;
//
// 	// Extract information about the Multiply callable object
// 	using multiply_traits = function_traits<Multiply>;
// 	std::cout << "\nReturn type of Multiply callable object: " << typeid(multiply_traits::return_type).name() << std::endl;
// 	std::cout << "Number of arguments for Multiply callable object: " << multiply_traits::arg_count << std::endl;
// 	std::cout << "Type of the second argument for Multiply callable object: " << typeid(multiply_traits::argument<1>::type).name() << std::endl;
//
// 	return 0;
// }

#pragma endregion

// function traits : callable 객체에 대한 정보를 추출하는 template-meta programming struct
// ex) return type, argument type, argument count 등등

// - T 가 function pointer 가 아닌 경우에 대한 template 특수화 이다.
//   T::operator() 라는 함수의 주소. 를 통해서 함수 정보를 추출한다.
template <typename T>
struct function_traits : function_traits<decltype(&T::operator())>
{
};

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
    static constexpr size_t arg_count = sizeof...(Args);

    using return_type = R;
    using arg_types = std::tuple<Args...>;

    // N th argument 정보를 추출하기 위한 struct
    // - std::tuple_element : 특정 idx 에 위치한 type 에 접근하기 위한 문법
    template <size_t N>
    struct argument
    {
        static_assert(N < arg_count, "error: invalid parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};


#pragma region>> function_traits function pointer, reference example
// template <typename T>
// struct function_traits;
//
// void myFunction(int a, double b) {}
//
// // Example function
// int add(int a, int b) {
// 	return a + b;
// }
//
// int main() {
// 	// Extract function pointer
// 	using FunctionTraits = function_traits<decltype(myFunction)>;
// 	static_assert(std::is_same<FunctionTraits::return_type, void>::value, "Return type should be void");
// 	static_assert(FunctionTraits::arg_count == 2, "Number of arguments should be 2");
// 	static_assert(std::is_same<FunctionTraits::argument<0>::type, int>::value, "First argument should be int");
// 	static_assert(std::is_same<FunctionTraits::argument<1>::type, double>::value, "Second argument should be double");
//
// 	// Extract information about the add function using a reference
// 	using AddFunctionType = decltype(add); // This is a reference to the function
// 	using AddFunctionTraits = function_traits<AddFunctionType>;
//
// 	std::cout << "Return type of add function: " << typeid(AddFunctionTraits::return_type).name() << std::endl;
// 	std::cout << "Number of arguments for add function: " << AddFunctionTraits::arg_count << std::endl;
// 	std::cout << "Type of the first argument for add function: " << typeid(AddFunctionTraits::argument<0>::type).name() << std::endl;
// 	std::cout << "Type of the second argument for add function: " << typeid(AddFunctionTraits::argument<1>::type).name() << std::endl;
//
// 	return 0;
// }
#pragma endregion

// 함수 포인터 type 에 대한 특수화
// 포인터를 unwrap 하고, 일반 함수인 것마냥 취급한다.
template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : function_traits<R(Args...)>
{
};

// 함수 Reference type 에 대한 특수화
// Reference를 unwrap 하고, 일반 함수인 것마냥 취급한다.
template <typename R, typename... Args>
struct function_traits<R (&)(Args...)> : function_traits<R(Args...)>
{
};


#pragma region>> function_traits member function example
// class MyClass {
// public:
// 	int add(int a, int b) {
// 		return a + b;
// 	}
//
// 	double multiply(double x, double y) const {
// 		return x * y;
// 	}
// };
//
// int main() {
// 	using AddFunctionType = decltype(&MyClass::add);
// 	using AddFunctionTraits = function_traits<AddFunctionType>;
//
// 	using MultiplyFunctionType = decltype(&MyClass::multiply);
// 	using MultiplyFunctionTraits = function_traits<MultiplyFunctionType>;
//
// 	std::cout << "Return type of add function: " << typeid(AddFunctionTraits::return_type).name() << std::endl;
// 	std::cout << "Class type for add function: " << typeid(AddFunctionTraits::class_type).name() << std::endl;
//
// 	std::cout << "Return type of multiply function: " << typeid(MultiplyFunctionTraits::return_type).name() << std::endl;
// 	std::cout << "Class type for multiply function: " << typeid(MultiplyFunctionTraits::class_type).name() << std::endl;
//
// 	return 0;
// }
#pragma endregion

// member function pointer 들에 대한 특수화
// non-const, const 함수
template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...)> : function_traits<R(Args...)>
{
    using class_type = C;
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) const> : function_traits<R(Args...)>
{
    using class_type = C;
};

// volatile non-const, const 함수
template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) volatile> : function_traits<R(Args...)>
{
    using class_type = C;
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) const volatile>
    : function_traits<R(Args...)>
{
    using class_type = C;
};

// std::function type 에 대한 특수화
// T : std::function 안에 담긴 callable object 의 type
// std::function 을 T callable object 처럼 다룬다.

#pragma region>> function_traits member function example

// int main() {
// 	// Define a function type and a std::function with the same signature
// 	using FunctionType = int(int);
// 	std::function<FunctionType> stdFunction = [](int x) { return x * 2; };
//
// 	// Use function_traits for std::function
// 	using StdFunctionTraits = function_traits<std::function<FunctionType>>;
// 	std::cout << "Result type for std::function: " << typeid(StdFunctionTraits::return_type).name() << std::endl;
// 	std::cout << "Argument types for std::function: ";
// 	std::tuple_element<0, StdFunctionTraits::arg_types>::type stdArgType;
// 	std::cout << typeid(stdArgType).name() << std::endl;
//
// 	return 0;
// }

#pragma endregion
template <typename T>
struct function_traits<std::function<T>> : function_traits<T>
{
};

template <typename... T>
struct type_count
{
    static constexpr auto size = sizeof...(T);
};

template <typename T, const char *const t_name>
struct type_name
{
public:
    static constexpr const char *Name()
    {
        return t_name;
    };
};

// reference : https://stackoverflow.com/questions/15036063/obtain-argument-index-while-unpacking-argument-list-with-variadic-templates?noredirect=1&lq=1

template <size_t... Is>
struct index_list
{
};

// Detail >>
// Declare primary template for index range builder
template <size_t MIN, size_t N, size_t... Is>
struct range_builder;

// Base step (1번째 특수화)
// - Min, Max 가 동일하면 indices 가 들어있지 않은 index_list 를 리턴한다.
template <size_t MIN, size_t... Is>
struct range_builder<MIN, MIN, Is...>
{
    typedef index_list<Is...> type;
};

// Induction step (2번째 특수화)
// N 을 Max 값에서 Min 값까지 1씩 감소시키면서 range_builder 를 계속해서 호출한다.
template <size_t MIN, size_t N, size_t... Is>
struct range_builder : public range_builder<MIN, N - 1, N - 1, Is...>
{
};

// <<

// Meta-function that returns a [MIN, MAX) index range
// 즉, [min, max) 라는 2개의 특정 값 사이에 존재하는 sequence of indices 를 만드는 데 사용되는
// meta-programming utility 이다.
// index_range : range_builder 를 recursive 하게 호출하여 원하는 결과값을 얻어온다.

#pragma region>> index_range example
// ex) index_range<3, 7> => 결과물 : index_list<3, 4, 5, 6>
// 3 : Min, 7 : Max
//
// range_builder<3, 7> =>
//
//
// 1) range_builder<3, 7, ...ls>
// -> range_builder<3, 6, 6, ls ...>
//
// 2) range_builder<3, 6, ...ls> (ls contains 6)
// -> range_builder<3, 5, 5, 6>
//
// 3) range_builder<3, 5, ...ls> (ls contains 5,6)
// -> range_builder<3, 4, 4, ls...>
//
// 3) range_builder<3, 4, ...ls> (ls contains 4,5,6)
// -> range_builder<3, 3, 3, ls...>
//
// 4) range_builder<3, 3, ...ls> (ls contains 3,4,5,6)
// (최종)
// 	template <size_t MIN, size_t... Is>
// 	struct range_builder<MIN, MIN, Is...>
// 	{
// 		typedef index_list<Is...> type;
// 	};
//
// 	여기서 ls... 는 empty 이다. 따라서 type 은 그저 empty index_list 가 된다.
//
// -> range_builder<3,4,5,6> (끝)
// -> 즉, range_builder<3,7> 에는 index_list<3,4,5,6> 가 type 이 된다.
#pragma endregion
template <size_t MIN, size_t MAX>
using index_range = typename range_builder<MIN, MAX>::type;

template <typename T, bool HasIterator>
struct get_iterator_impl
{
};

template <typename T>
struct get_iterator_impl<T, false>
{
    using type = void;
};

template <typename T>
struct get_iterator_impl<T, true>
{
    using type = typename T::Iterator;
};

template <typename T>
struct get_iterator
{
    // T class 에 begin() 멤버 함수가 있고 + void 리턴형태라면,
    // ::type 은 T::Iterator 가 되고 / 아니라면 void 가 된다.
    using type = typename get_iterator_impl<
        T,
        std::is_void<decltype(std::declval<T>().begin())>::value>::type;
};

// https://github.com/rttrorg/rttr/blob/6bf5b572b67a9793f457e9028d4e2b1fe5f7de69/src/rttr/detail/misc/misc_type_traits.h

#pragma region>> pointer_count example
// using Type1 = int;
// using Type2 = int*;
// using Type3 = int**;
// using Type4 = int***;
//
// std::cout << pointer_count<Type1>::value << std::endl; // 0
// std::cout << pointer_count<Type2>::value << std::endl; // 1
// std::cout << pointer_count<Type3>::value << std::endl; // 2
// std::cout << pointer_count<Type4>::value << std::endl; // 3
//
// return 0;
#pragma endregion
template <typename T, typename Enable = void>
struct pointer_count_impl
{
    static constexpr size_t size = 0;
};

template <typename T>
struct pointer_count_impl<
    T,
    typename std::enable_if<std::is_pointer<T>::value>::type>
{
    static constexpr size_t size =
        pointer_count_impl<typename std::remove_pointer<T>::type>::size + 1;
};

template <typename T>
using pointer_count =
    std::integral_constant<std::size_t, pointer_count_impl<T>::size>;


template <typename C>
struct param_traits
{
    using type = typename std::remove_reference<C>::type;
};

template <typename T, typename U>
constexpr size_t offset_of(U T::*member)
{
    // (T*)nullptr						: nullptr 을 T* 로 임시 캐스팅함으로써 T type 의 temporary object 생성
    // (T*)nullptr->*member				: 이후 해당 object 의 member 변수에 접근
    // (char*)&((T*)nullptr->*member)   : 해당 멤버 변수의 byte address 정보를 가져온다.
    // (char*)nullptr:					: null byte address 정보를 가져온다.
    // 최종 결과 : T Class 에서 해당 member 변수의 offset 을 가져온다.
    return (char *)&((T *)nullptr->*member) - (char *)nullptr;
}

template <class T, std::size_t = sizeof(T)>
std::true_type is_complete_impl(T *);

std::false_type is_complete_impl(...);

template <class T>
using is_complete = decltype(is_complete_impl(std::declval<T *>()));


template <class T, class EqualTo>
struct has_operator_equal_impl
{
    template <class U, class V>
    static auto test(U *) -> decltype(std::declval<U>() == std::declval<V>());
    template <typename, typename>
    static auto test(...) -> std::false_type;

    using type =
        typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
};

template <class T, class EqualTo = T>
struct has_operator_equal : has_operator_equal_impl<T, EqualTo>::type
{
};


template <class T, class... TArgs>
struct iterator
{
    using next = iterator<TArgs...>;
    static const constexpr std::size_t size = 1 + next::size;

#if __cplusplus > 201402L
    template <class TFunc>
    inline constexpr static TFunc for_each(TFunc callback,
                                           T &&t,
                                           TArgs &&...args)
    {
        callback(std::forward<T>(t));
        next::for_each(callback, std::forward<TArgs>(args)...);
        return callback;
    }
#endif

    template <class TFunc>
    inline constexpr TFunc operator()(TFunc callback,
                                      T &&t,
                                      TArgs &&...args) const
    {
        return for_each(callback,
                        std::forward<T>(t),
                        std::forward<TArgs>(args)...);
    }
};

template <class T>
struct iterator<T>
{
    static const constexpr std::size_t size = 1;

#if __cplusplus > 201402L
    template <class TFunc>
    inline constexpr static TFunc for_each(TFunc callback, T &&t)
    {
        callback(std::forward<T>(t));
        return callback;
    }
#endif

    template <class TFunc>
    inline constexpr TFunc operator()(TFunc callback, T &&t) const
    {
        return for_each(callback, std::forward<T>(t));
    }
};
