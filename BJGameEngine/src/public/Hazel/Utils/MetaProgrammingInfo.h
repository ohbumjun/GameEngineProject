#pragma once

#include "hzpch.h"

// Reference : https://github.com/electronicarts/EASTL/blob/master/include/EASTL/bonus/call_traits.h

template <typename T, bool isSmall>
struct call_type_impl2
{
    typedef const T &ParamType;
};

// isSmall �� true �� �Ϳ� ���� ���ø� Ư��ȭ
template <typename T>
struct call_type_impl2<T, true>
{
    typedef const T ParamType;
};

/*
(��� ����)
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

// T �� ������								: const T
// T �� �����Ͱ� �ƴ� && 4 byte ���� ũ��	: const T&
// ���� : sizeof(void*) = 32 bit ? 4 byte / 64 bit ? 8 byte;
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
    // std::is_arithmetic<T>::value : arithmethic type �� �ش��ϸ� true / false
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

// call_traits �� ���� & type Ư��ȭ
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

// - (std::true_type) �� ��ӹް�
// - T class �� operator() ����� ���� Ư��ȭ �̴�.
// T �� operator () �� �����ϴ����� �˻��Ѵ�.
// decltype(void(&T::operator())) �� operator () �� ����Ÿ���� �����Ѵ�.
// void(&T::operator()) �� operator () ��� �Լ���, �ּҸ� �������� ���� ǥ���̰�
// decltype �� �������ν� ����Ÿ���� Ȯ���� �� �ִ�.
// ���� operator () �� �����ϰ�, void ����Ÿ���̶��, T �� �Լ�ó�� ���� �� �ִ�.
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


// - �������� ���ø��� ���� �ǹ��ϴ� ǥ���̴�.
//   ��, T ���� ���̻��� template ���ڰ� �־����� �ʴ´ٸ�
//   Primary template �� �⺻������ ���ȴٴ� ���̴�.
// template <typename T, typename ...> struct is_base_of_any : std::true_type {};
template <typename T, typename...>
struct is_base_of_any : std::false_type
{
};

// - is_base_of_any �� ���� �������� ���ø�
// - std::is_base_of<T, Head>::value : T �� Head �� Base Class ���� Ȯ���Ѵ�. (Head �� T ���)
// - std::integral_constant<bool, std::is_base_of<T, Head>::value ?
//		������ Ÿ�� �� std::integral_constant<bool, Value> �� ���� compile-time constant type �� �����
//		���� ���̴�.
//   ��, std::is_base_of<T, Head>::value ��
//   1) true ��� -> std::integral_constant<bool, std::is_base_of<T, Head>::value �� std::true_type
//   2) false��� -> std::false_type

// - is_base_of_any<T, Rest...>::value �� T �� ������ template ���� Ÿ�Ե��� Base Class ������
//   ��������� �˻��ϴ� �����̴�.
//   ��, Rest ... �� �ִ� type �߿��� �ϳ���, T �� base class ���, �� t �� �Ļ�Ŭ�������
//   true_type �� �ȴ�.

//  ��� : T �� ������ ���ø� Ŭ���� �߿��� T �� �Ļ� Ŭ������ �ϳ��� �����ϴ����� �˻��ϴ� �ڵ�
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
//   => 1) C Class �� begin() ��� �Լ��� �����ϰ�
//   => 2) void �� �����Ѵٸ� true. �׿ܿ��� false

// - !std::is_same<decltype(std::declval<C>().begin()), void>::value &&
//   !std::is_same<decltype(std::declval<C>().end()), void>::value
//   => begin(), end() �Լ� �Ѵ� �����ϵ�, �Ѵ� void �� �ƴ� Ÿ���� �����Ѵٸ� true
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
    // remove_all_extents : array ���� ��� ���� ex) int[3][4] -> int
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
// 	// ������ ������ ����.
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

// structure �� �����ϴ� ���漱���� ��, �ش� struct �� ���� � ���ǵ� �������� �ʴ´�.
template <typename T, typename... Ts>
struct get_index;

// - �ڱ� �ڽſ� ���� Ư��ȭ. �������� ���ø� ���¿��� T,T,... �̷��� ���� template ��
//	 2�� ���� ���� ��쿡 �ش��Ѵ�.
// - std::integral_constant<std::size_t, 0> {};
//   std::size_t Ÿ���� ������ Ÿ�� constant �� ǥ���ϱ� ���� ��������, �� ��� 0 �� ��Ÿ����.
//   ��, T Ÿ���� type ���� list �� �߿��� 0��° idx �� ��ġ�Ѵٶ�� ���� �ǹ��Ѵ� .
template <typename T, typename... Ts>
struct get_index<T, T, Ts...> : std::integral_constant<std::size_t, 0>
{
};

// T ����, T �� ã�Ƽ� ����ؼ� ��������� get_index �� ã�ư��� ���̴�.
template <typename T, typename Tail, typename... Ts>
struct get_index<T, Tail, Ts...>
    : std::integral_constant<std::size_t, 1 + get_index<T, Ts...>::value>
{
};

// Type list �� ���� T �ϳ� Ȥ��, empty �� ��쿡 ���� Ư��ȭ �̴�.
template <typename T>
struct get_index<T>
{
    // ���⿡ �ɸ���, �׻� ������ Ÿ�� ������ ������ ���̴�.
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

// function traits : callable ��ü�� ���� ������ �����ϴ� template-meta programming struct
// ex) return type, argument type, argument count ���

// - T �� function pointer �� �ƴ� ��쿡 ���� template Ư��ȭ �̴�.
//   T::operator() ��� �Լ��� �ּ�. �� ���ؼ� �Լ� ������ �����Ѵ�.
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

    // N th argument ������ �����ϱ� ���� struct
    // - std::tuple_element : Ư�� idx �� ��ġ�� type �� �����ϱ� ���� ����
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

// �Լ� ������ type �� ���� Ư��ȭ
// �����͸� unwrap �ϰ�, �Ϲ� �Լ��� �͸��� ����Ѵ�.
template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : function_traits<R(Args...)>
{
};

// �Լ� Reference type �� ���� Ư��ȭ
// Reference�� unwrap �ϰ�, �Ϲ� �Լ��� �͸��� ����Ѵ�.
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

// member function pointer �鿡 ���� Ư��ȭ
// non-const, const �Լ�
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

// volatile non-const, const �Լ�
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

// std::function type �� ���� Ư��ȭ
// T : std::function �ȿ� ��� callable object �� type
// std::function �� T callable object ó�� �ٷ��.

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

// Base step (1��° Ư��ȭ)
// - Min, Max �� �����ϸ� indices �� ������� ���� index_list �� �����Ѵ�.
template <size_t MIN, size_t... Is>
struct range_builder<MIN, MIN, Is...>
{
    typedef index_list<Is...> type;
};

// Induction step (2��° Ư��ȭ)
// N �� Max ������ Min ������ 1�� ���ҽ�Ű�鼭 range_builder �� ����ؼ� ȣ���Ѵ�.
template <size_t MIN, size_t N, size_t... Is>
struct range_builder : public range_builder<MIN, N - 1, N - 1, Is...>
{
};

// <<

// Meta-function that returns a [MIN, MAX) index range
// ��, [min, max) ��� 2���� Ư�� �� ���̿� �����ϴ� sequence of indices �� ����� �� ���Ǵ�
// meta-programming utility �̴�.
// index_range : range_builder �� recursive �ϰ� ȣ���Ͽ� ���ϴ� ������� ���´�.

#pragma region>> index_range example
// ex) index_range<3, 7> => ����� : index_list<3, 4, 5, 6>
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
// (����)
// 	template <size_t MIN, size_t... Is>
// 	struct range_builder<MIN, MIN, Is...>
// 	{
// 		typedef index_list<Is...> type;
// 	};
//
// 	���⼭ ls... �� empty �̴�. ���� type �� ���� empty index_list �� �ȴ�.
//
// -> range_builder<3,4,5,6> (��)
// -> ��, range_builder<3,7> ���� index_list<3,4,5,6> �� type �� �ȴ�.
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
    // T class �� begin() ��� �Լ��� �ְ� + void �������¶��,
    // ::type �� T::Iterator �� �ǰ� / �ƴ϶�� void �� �ȴ�.
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
    // (T*)nullptr						: nullptr �� T* �� �ӽ� ĳ���������ν� T type �� temporary object ����
    // (T*)nullptr->*member				: ���� �ش� object �� member ������ ����
    // (char*)&((T*)nullptr->*member)   : �ش� ��� ������ byte address ������ �����´�.
    // (char*)nullptr:					: null byte address ������ �����´�.
    // ���� ��� : T Class ���� �ش� member ������ offset �� �����´�.
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
