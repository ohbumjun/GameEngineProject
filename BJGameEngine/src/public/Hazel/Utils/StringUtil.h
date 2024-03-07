#pragma once

#include "hzpch.h"


namespace Utils
{


#define INIT_STR_SHORT_LENGTH 256
#define INIT_STR_LENGTH 512
#define INIT_STR_LONG_LENGTH 2048

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline U pr_str_tolower(const U c)
{
    return tolower(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline U pr_str_tolower(const U c)
{
    return towlower(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline U pr_str_toupper(const U c)
{
    return toupper(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline U pr_str_toupper(const U c)
{
    return towupper(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline U *pr_strdup(const U *c)
{
    return strdup(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline U *pr_strdup(const U *c)
{
    return wcsdup(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline size_t pr_strlen(const U *c)
{
    return strlen(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline size_t pr_strlen(const U *c)
{
    return wcslen(c);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline void pr_strcpy(U *dest, const U *src)
{
    strcpy(dest, src);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline void pr_strcpy(U *dest, const U *src)
{
    wcscpy(dest, src);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline void pr_strncpy(U *dest, const U *src, size_t len)
{
    strncpy(dest, src, len);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline void pr_strncpy(U *dest, const U *src, size_t len)
{
    wcsncpy(dest, src, len);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline int pr_strncmp(const U *a, const U *b, size_t count)
{
    if (count == 0)
        return 0;

    if (a == nullptr && b == nullptr)
        return 0;

    if (a == nullptr)
        return -1;
    if (b == nullptr)
        return 1;

    return strncmp(a, b, count);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline int pr_strncmp(const U *a, const U *b, size_t count)
{
    if (count == 0)
        return 0;

    if (a == nullptr && b == nullptr)
        return 0;

    if (a == nullptr)
        return -1;
    if (b == nullptr)
        return 1;

    return wcsncmp(a, b, count);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline int pr_strcmp(const U *a, const U *b)
{
    if (a == nullptr && b == nullptr)
        return 0;

    if (a == nullptr)
        return -1;
    if (b == nullptr)
        return 1;

    return strcmp(a, b);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline int pr_strcmp(const U *a, const U *b)
{
    return wcscmp(a, b);
}


/*
 @brief 대소문자를 구분하지 않고 string 비교합니다.
 @param s1 비교할 문자열1
 @param s2 비교할 문자열2
 @return 0			s1과 s2 동일
         음수(< 0)	s1이 s2보다 길이가 더 작다.
         양수(> 0)	s1이 s2보다 길이가 더 크다.
*/
template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline int pr_strcmp_no_case(const U *a, const U *b)
{
    return strcasecmp(a, b);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline int pr_strcmp_no_case(const U *a, const U *b)
{
    return wcscasecmp(a, b);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
int inline pr_strncmp_no_case(const U *a, const U *b, size_t count)
{
    return strncasecmp(a, b, count);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
int inline pr_strncmp_no_case(const U *a, const U *b, size_t count)
{
    return wcsncasecmp(a, b, count);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline U *pr_strcat(U *a, const U *b)
{
    return strcat(a, b);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline U *pr_strcat(U *a, const U *b)
{
    return wcscat(a, b);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline U *pr_strstr(U *str, const U *strSearch)
{
    return strstr(str, strSearch);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline const U *pr_strstr(const U *str, const U *strSearch)
{
    return strstr(str, strSearch);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline U *pr_strstr(U *str, const U *strSearch)
{
    return wcsstr(str, strSearch);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline const U *pr_strstr(const U *str, const U *strSearch)
{
    return wcsstr(str, strSearch);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
inline int pr_vsnprintf(U *target, size_t n, const U *format, va_list arg_ptr)
{
    return vsnprintf(target, n, format, arg_ptr);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
inline int pr_vsnprintf(U *target, size_t n, const U *format, va_list arg_ptr)
{
    return vswprintf(target, n, format, arg_ptr);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
size_t pr_strspn(const U *str, const U *ch)
{
    return strspn(str, ch);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, char>::value>::type * = nullptr>
size_t pr_strcspn(const U *str, const U *ch)
{
    return strcspn(str, ch);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
size_t pr_strspn(const U *str, const U *ch)
{
    return wcsspn(str, ch);
}

template <
    typename U,
    typename std::enable_if<std::is_same<U, wchar_t>::value>::type * = nullptr>
size_t pr_strcspn(const U *str, const U *ch)
{
    return wcscspn(str, ch);
}

// find first occurence of substr in src from back to front
// https://stackoverflow.com/questions/1634359/is-there-a-reverse-function-for-strstr
template <typename T>
T *pr_strrstr(T *src, const T *substr)
{
    if (*substr == '\0')
        return static_cast<T *>(src);

    T *r = NULL;
    for (;;)
    {
        T *p = pr_strstr(src, substr);
        if (p == NULL)
            break;
        r = p;
        src = p + 1;
    }

    return r;
}

template <typename T>
const T *pr_strrstr(const T *src, const T *substr)
{
    if (*substr == '\0')
        return static_cast<const T *>(src);

    const T *r = NULL;
    for (;;)
    {
        const T *p = pr_strstr(src, substr);
        if (p == NULL)
            break;
        r = p;
        src = p + 1;
    }

    return r;
}

template <typename U>
U *pr_strtok(U *str, const U *delim)
{
    static U *p = 0;
    if (str)
        p = str;
    else if (!p)
        return 0;
    str = p + pr_strspn(p, delim);
    p = str + pr_strcspn(str, delim);
    if (p == str)
        return p = 0;

    p = *p ? *p = 0, p + 1 : 0;
    return str;
}

template <typename U>
int pr_str_last_index_of(const U *str, U ch)
{
    int p = 0;
    int index = -1;
    while (str[p])
    {
        if (str[p] == ch)
        {
            index = p;
        }

        ++p;
    }

    return index;
}


/*
유니코드 : 통일된 문자 체계
해당 문자에 부여된 고유 숫자들이 존재하기는 한다.
하지만 그 숫자를 표현하기 위해서 몇바이트를 할당하는가가 인코딩 방식이라고 할 수 있다.

ex) A = 65 라고 하더라도, 이것을 1 바이트로 표현할 것인지, 2바이트로 표현할 것인지
     가 인코딩 방식마다 다른 것이다.

ex) '널' -> B110 (한글자당 4 bit) -> 총 16 bit

유니코드에서는 결국 문자마다 할당한 바이트 숫자가 다르다
이거를 'MBCS (multibyte character set) 이라고 부르기도 한다.

즉, 유니코드랑 MBCS 는 거의 동일한 의미라고 생각하면 된다.
*/

/*
UTF-8

인코딩 방식 중 하나다.
'유니코드' 에 대해서 한글은 '3' byte 로 변환한다

ex) '널' -> 유니코드 'B110' 으로 16 bit -> 그런데 이거를
     8bit 8bit 8bit 로 사용한다.

2 바이트 이상으로 표신된 문자의 경우, 첫 바이트의 상위 비트들이 그 문자를
표시하는데 필요한 바이트 수를 결정한다.
ex) 110 로 시작 == 2 바이트       : 110xxxx
ex) 1110 으로 시작 == 3바이트   : 1110xxx

그리고 첫바이트가 아닌 나머지 바이트들은 상위 2bit 가 항상 10 이다.
*/

/*
<참고>
LSB : Least Significant Bit
가장 낮은 위치의 Bit
2^0 의 위치에 존재한다.
[][][][][][][][LSB]

MSB : Most Significant Bit
가장 최상위 위치의 Bit

- unsigned int -
2^7 의 위치에 존재.
[MSB][][][][][][][]

- singed    int :
2^7 의 위치에 존재.
[MSB == 부호 비트][][][][][][][]
즉, singed int 에서 MSB 를 보면 음수인지 양수인지를 알 수 있다.
    */

class StringUtil
{
public:
    static void CopyStr(char *const dst, size_t dstSize, const char *src);

    /*dst 에 src string 을 뒤에 붙이는 함수*/
    static void StrcatStr(char *dst, const char *src, size_t dstSize);

    static char *ReplaceStr(char const *const pattern,
                            char const *const replace,
                            char const *const src);

    static void ReplaceStr(std::string &str, char target, char replacement);

    static void ReplaceStrToDst(char *dest,
                                char const *const src,
                                char const *const pattern,
                                char const *const replace);

    /*
    * non-ascii 문자를 가지고 있는 utf-8 문자열이 필요한 추가 length 를 계산한다.
    */
    // static int pr_str_utf8_extra_length(const char* src_utf8);
    static int CalculateUTF8ExtraLength(const char *src_utf8);

    static int pr_str_wchar_to_utf8(char *dest, uint32 ch);

    static int pr_str_utf8_read_escape_sequence(const char *src, uint32 *dest);

    static int pr_str_utf8_unescape(char *buf, int sz, const char *src);

    static bool pr_str_is_utf8(const char *c);

    static std::string pr_str_system_to_utf8(const char *cp);

    static void ReplaceWString(std::wstring &str,
                               char target,
                               char replacement);

    static std::vector<std::string> p_split_string(const std::string &origin,
                                                   char split);

#pragma region trim_example
    /*
#include <algorithm>
#include <iostream>
#include <string>

    // Trim from beginning (left)
    std::string trimStart(const std::string& str, char c) {
        size_t start = str.find_first_not_of(c);
        if (start == std::string::npos) {
            return "";  // Entire string consists of 'c'
        }
        return str.substr(start);
    }

    // Trim from end (right)
    std::string trimEnd(const std::string& str, char c) {
        size_t end = str.find_last_not_of(c);
        if (end == std::string::npos) {
            return "";  // Entire string consists of 'c'
        }
        return str.substr(0, end + 1);
    }

    // Trim from both beginning and end
    std::string trim(const std::string& str, char c) {
        return trimEnd(trimStart(str, c), c);
    }

    int main() {
        std::string text = "   Hello, world!   ";
        char targetChar = ' ';

        std::string trimmed = trim(text, targetChar);
        std::string trimmedStart = trimStart(text, targetChar);
        std::string trimmedEnd = trimEnd(text, targetChar);

        std::cout << "Original: '" << text << "'" << std::endl;
        std::cout << "Trimmed: '" << trimmed << "'" << std::endl;
        std::cout << "Trimmed Start: '" << trimmedStart << "'" << std::endl;
        std::cout << "Trimmed End: '" << trimmedEnd << "'" << std::endl;

        return 0;
    }
Original: '   Hello, world!   '
Trimmed : 'Hello, world!'
Trimmed Start : 'Hello, world!   '
Trimmed End : '   Hello, world!'
*/
#pragma endregion

    static std::string pr_trim_start(const std::string &str, char c);

    static std::string pr_trim_end(const std::string &str, char c);

    static std::string pr_trim(const std::string &str, char c);

    static std::wstring pr_trim_start(const std::wstring &str, char c);

    static std::wstring pr_trim_end(const std::wstring &str, char c);

    static std::wstring pr_trim(const std::wstring &str, char c);
};

} // namespace Utils
