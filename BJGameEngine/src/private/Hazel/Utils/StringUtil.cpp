#include "Hazel/Utils/StringUtil.h"
#include "hzpch.h"

namespace Utils
{


void StringUtil::CopyStr(char *const dst, size_t dstSize, const char *src)
{
    size_t srcLength = strlen(dst);

    if (srcLength > dstSize)
    {
        assert(false, "The srcs size less than dst size");
    }

    strcpy(dst, src);
}

void StringUtil::StrcatStr(char *dst, const char *src, size_t dstSize)
{
    strcat_s(dst, dstSize, src);
}

char *StringUtil::ReplaceStr(char const *const pattern,
                             char const *const replace,
                             char const *const src)
{
    size_t patternCnt = 0;
    const char *originalPtr;
    const char *patternLocation;

    size_t const originalLen = strlen(src);
    size_t const replen = strlen(replace);
    size_t const patternlen = strlen(pattern);

    // find how many times the pattern occurs in the original string
    for (originalPtr = src; (patternLocation = strstr(originalPtr, pattern));
         originalPtr = patternLocation + patternlen)
    {
        // strstr searches for the first occurrence of pattern within oriptr (the remaining part of the string)
        // If strstr finds a match (patloc is not NULL), it means a pattern was found in the remaining part of the string.
        // 만약 Null 을 리턴하게 되면 for 문을 빠져나오게 될 것이다.
        // 참고 : strstr 의 리턴값 == oriptr 위치에서 시작해서 처음 발견된 pattern 의 첫번째 character 에 대한 포인터.
        // 즉, 매번 for 문을 돌 때마다 찾은 pattern 끝부분에서 다시 pattern 에 대한 탐색을 반복하는 구조이다.

        // oriptr 은 바로 그 다음 pattern 시작점 위치로 이동한다.

        // patcnt 는 pattern 의 발견횟수만큼 증가한다.
        patternCnt++;
    }

    {
        // allocate memory for the new string
        size_t const replacedLen =
            originalLen + patternCnt * (replen - patternlen);
        char *const replacedChar =
            static_cast<char *>(malloc(sizeof(char) * (replacedLen + 1)));

        if (replacedChar != NULL)
        {
            // copy the original string,
            // replacing all the instances of the pattern
            char *replacedPtr = replacedChar;

            for (originalPtr = src;
                 (patternLocation = strstr(originalPtr, pattern));
                 originalPtr = patternLocation + patternlen)
            {
                size_t const skplen = patternLocation - originalPtr;

                // copy the section until the occurence of the pattern
                strncpy(replacedPtr, originalPtr, skplen);
                replacedPtr += skplen;

                // copy the replacement
                strncpy(replacedPtr, replace, replen);
                replacedPtr += replen;
            }

            // copy the rest of the string.
            strcpy(replacedPtr, originalPtr);
        }

        return replacedChar;
    }
}

void StringUtil::ReplaceStr(std::string &str, char target, char replacement)
{
    std::replace(str.begin(), str.end(), target, replacement);
}

void StringUtil::ReplaceStrToDst(char *dest,
                                 char const *const src,
                                 char const *const pattern,
                                 char const *const replace)
{
    size_t const replen = strlen(replace);
    size_t const patlen = strlen(pattern);

    size_t patcnt = 0;
    const char *oriptr;
    const char *patloc;

    // find how many times the pattern occurs in the original string
    for (oriptr = src; (patloc = strstr(oriptr, pattern));
         oriptr = patloc + patlen)
    {
        patcnt++;
    }

    {
        if (dest != NULL)
        {
            // copy the original string,
            // replacing all the instances of the pattern
            char *retptr = dest;
            for (oriptr = src; (patloc = strstr(oriptr, pattern));
                 oriptr = patloc + patlen)
            {
                size_t const skplen = patloc - oriptr;
                // copy the section until the occurence of the pattern
                strncpy(retptr, oriptr, skplen);
                retptr += skplen;
                // copy the replacement
                strncpy(retptr, replace, replen);
                retptr += replen;
            }
            // copy the rest of the string.
            strcpy(retptr, oriptr);
        }
    }
}

void StringUtil::ReplaceWString(std::wstring &str,
                                char target,
                                char replacement)
{
    std::replace(str.begin(), str.end(), target, replacement);
}

/*
void StringUtil::pr_strscat(char* dest, const char* src, size_t destinationSize)
{
	size_t len = strlen(src);

	if (len > destinationSize)
	{
		if (destinationSize) dest[0] = 0;

		return;
	}

	StrcatStr(dest, src, destinationSize);
}
*/


int StringUtil::CalculateUTF8ExtraLength(const char *src_utf8)
{
    /*
	const char* utf8String = "Hello, 안녕하세요"; // UTF-8 string with ASCII and non-ASCII characters
	결과물 '5'

	우선, 현재 함수의 취지가 정확하게 무엇인지는 모르겠다.
	UTF8 형태로 변환할 때, 총 몇개의 byte 가 필요한지에 대한 정보로 보인다.

	기본적으로 Windows 는 WBCS 을 사용한다고 알고 있다.
	그러면 한글은 2 byte 로 표현하는데

	그것을 UTF-8 형태로 변경한다고 하면
	5 개의 한글에 대해 1 byte 씩 더 사용해야 하니
	총 5 byte 로 예상한다.

	나는 10 byte 가 필요할 것이라고 생각했다.
	str 은 1 byte 로 한글을 표현하는데
	3 byte 로 해야 하니 2 * 5 = 10

	하지만, 정확한 이유가 무엇인지는 잘 모르겠다.
*/

    size_t srcLength = strlen(src_utf8);
    int addedCnt = 0;

    // 인자로 들어온 문자열에서 각 문자를 순회한다.
    for (size_t i = 0; i < srcLength; ++i)
    {
        /*
		만약 character'code 가 음수라면 utf-8 인코딩에서 non-ascii character 임을 의미한다.

		<설명>
		UTF-8 에서는 ASCII 문자 (0 ~ 127) 는 single byte 로 표현된다.
		하지만 non-ascii 같은 경우 , multiple byte 로 표현된다.

		특히 UTF-8 에서 multiple byte 에서 첫번째 byte 는 
		MSB (가장 왼쪽 bit) 는 '1' 이다.

		즉, UTF- 8 을 byte 단위로 해석할 때, 
		ASCII 는 MSB 가 0 이다.
		NON-ASCII 는 MSB 0 이 아니라. '1' 일 수 있다.
		모든 NON-ASCII 가 '1' 인 것은 아니라는 의미.
		
		(다른 말로 , 아스키는 signed, non아스키는 unsigned
		 라는 의미로 보인다)

		그런데 C/C++ 에서는 char 로 문자를 표현하게 되는데
		char 가 구현된, 혹은 구성된 방식에 따라서
		정상적으로 non-Ascii 를 판단하기 어려울 수도 있다.

		예를 들어, char 가 signed 이고 2의 보수 형태로 구현되어 있다.
		그러면 signed char 는 -128 ~ 127 범위이고
		127 보다 큰 숫자를 음수로 판단된다.

		예를 들어, 'A' 라는 ASCII 는 0x41 bit 로 표현되고
		MSB 가 '0' 이다. 따라서 char ~ = 'A' 는 non-negative 이다.

		반면 UTF-8 에서 '한' 이라는 한글은 
		0xED 0x95 0x9C 인데, 이때 0xED 에서 MSB 는 '1' 이다.

		char ~ = '0xED' 를 하게 되면
		음수로 판별된다.

		<예시>
		char asciiChar = 'A'; // ASCII character
		char nonAsciiChar = '\xED'; // Non-ASCII character as an example byte

		if (asciiChar < 0) {
			std::cout << "ASCII character code is considered negative in this implementation." << std::endl;
		} else {
			std::cout << "ASCII character code is non-negative in this implementation." << std::endl;
		}

		if (nonAsciiChar < 0) {
			std::cout << "Non-ASCII character code is considered negative in this implementation." << std::endl;
		} else {
			std::cout << "Non-ASCII character code is non-negative in this implementation." << std::endl;
		}

		*/
        if (static_cast<int>(src_utf8[i]) < 0)
        {
            /*
			여기로 들어온 다는 것은, UTF-8 문자열 중에서 non-ascii 코드임을 의미한다.
			src_utf8[i] 는 1 bit 가 아니라, 1 byte 이다. (확실하지는 않지만, test 디버깅 상 그렇다)
			static_cast<uint>(src_utf8[i]) 를 uint 정수로 형변환 한다.
			% 0x100 : 하위 8 bit 이외에 모든 bit 를 무시하기 위한 연산이다.
			0x100 : 0000,0001,0000,0000
			*/
            int lower8Bit = static_cast<uint>(src_utf8[i]) % 0x100;
            int addedBytes = 0;

            // 0x40 : 01000000 -> 00000100
            // 0x20 : 00100000 -> 00000010
            // 0x10 : 00010000 -> 00000001
            // flags 는 0000,0111 ~ 0000,0000 사이의 숫자로 표현된다.
            // 즉 최상위 비트를 제외하고, 그 이후 3 개 bit 구성을 살핀다.
            int flags = ((lower8Bit & 0x40) >> 4) + ((lower8Bit & 0x20) >> 4) +
                        ((lower8Bit & 0x10) >> 4);

            // '0b' : 인진수임을 표현하는 표시.
            // 0b0100  : 00000100
            // 0b0010  : 00000010
            // 아래 과정은 이제 해당 non-ascii 문자를 표현하기 위해서 사용된 additional byte 개수를 구하고자 하는 것이다.
            if (flags & 0b0100)
            {
                if (flags & 0b0010)
                {
                    if (flags & 1)
                        addedBytes = 3;
                    else
                        addedBytes = 2;
                }
                else
                    addedBytes = 1;
            }

            addedCnt += addedBytes;

            // 추가된 byte 만큼 건너뛰어서, 그 다음 문자를 검사할 수 있게 해주는 것이다.
            i += addedBytes;
        }
    }
    return addedCnt;
}

int StringUtil::ChangeWCharToUTF8(char *dest, uint32 ch)
{
    if (ch < 0x80)
    {
        dest[0] = (char)ch;
        return 1;
    }
    if (ch < 0x800)
    {
        dest[0] = (ch >> 6) | 0xC0;
        dest[1] = (ch & 0x3F) | 0x80;
        return 2;
    }
    if (ch < 0x10000)
    {
        dest[0] = (ch >> 12) | 0xE0;
        dest[1] = ((ch >> 6) & 0x3F) | 0x80;
        dest[2] = (ch & 0x3F) | 0x80;
        return 3;
    }
    if (ch < 0x110000)
    {
        dest[0] = (ch >> 18) | 0xF0;
        dest[1] = ((ch >> 12) & 0x3F) | 0x80;
        dest[2] = ((ch >> 6) & 0x3F) | 0x80;
        dest[3] = (ch & 0x3F) | 0x80;
        return 4;
    }
    return 0;
}

int StringUtil::pr_str_utf8_read_escape_sequence(const char *src, uint32 *dest)
{
    return 1;
}

int StringUtil::pr_str_utf8_unescape(char *buf, int sz, const char *src)
{
    int c = 0, amt;
    uint32 ch;
    char temp[4];

    while (*src && c < sz)
    {
        if (*src == '\\')
        {
            src++;
            amt = pr_str_utf8_read_escape_sequence(src, &ch);
        }
        else
        {
            ch = (uint32)*src;
            amt = 1;
        }
        src += amt;
        amt = pr_str_wchar_to_utf8(temp, ch);
        if (amt > sz - c)
            break;
        memcpy(&buf[c], temp, amt);
        c += amt;
    }
    if (c < sz)
        buf[c] = '\0';
    return c;

    return 1;
}

bool StringUtil::IsStrUTF8(const char *c)
{
    for (size_t i = 0; i < strlen(c); ++i)
    {
        int num;
        char temp = c[i];
        if ((temp & 0x80) == 0x00)
        {
            // U+0000 to U+007F
            num = 1;
        }
        else if ((temp & 0xE0) == 0xC0)
        {
            // U+0080 to U+07FF
            num = 2;
        }
        else if ((temp & 0xF0) == 0xE0)
        {
            // U+0800 to U+FFFF
            num = 3;
        }
        else if ((temp & 0xF8) == 0xF0)
        {
            // U+10000 to U+10FFFF
            num = 4;
        }
        else
            return false;

        for (int j = 1; j < num; ++j)
        {
            char t = c[++i];
            if ((t & 0xC0) != 0x80)
                return false;
        }
    }
    return true;
}

std::string StringUtil::ChangeSystemStrToUTF8(const char *cp)
{
    const auto inStr = (const wchar_t *)cp;
    const size_t cpSize = strlen(cp);
    const int bufferSize = NormalizeString(NormalizationC,
                                           inStr,
                                           static_cast<int>(cpSize),
                                           nullptr,
                                           0);
    // const LvWString buffer(bufferSize);
    std::wstring buffer;
    buffer.resize(bufferSize);

    if (!pr_str_is_utf8(cp))
    {
        MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            cp,
            -1,
            (LPWSTR)buffer.c_str(),
            static_cast<int>(cpSize + CalculateUTF8ExtraLength(cp)));
    }

    // Normalize.
    NormalizeString(NormalizationC,
                    inStr,
                    static_cast<int>(cpSize),
                    (LPWSTR)buffer.c_str(),
                    static_cast<int>(buffer.length()));
    const int size = WideCharToMultiByte(CP_UTF8,
                                         WC_NO_BEST_FIT_CHARS,
                                         buffer.c_str(),
                                         -1,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL);
    // LvString utf8(size);
    std::string utf8;
    utf8.resize(size);

    if (size > 0)
    {
        WideCharToMultiByte(CP_UTF8,
                            WC_NO_BEST_FIT_CHARS,
                            buffer.c_str(),
                            -1,
                            (char *)utf8.c_str(),
                            size,
                            NULL,
                            NULL);
        return utf8;
    }

    return std::string();
}


std::vector<std::string> StringUtil::SplitString(const std::string &origin,
                                                    char split)
{
    char delimiter = ',';

    std::vector<std::string> substrings;

    size_t startPos = 0; // Starting position for searching

    while (startPos < origin.length())
    {

        size_t foundPos = origin.find(delimiter, startPos);

        if (foundPos == std::string::npos)
        {
            // If the delimiter is not found, add the remaining part of the string
            substrings.push_back(origin.substr(startPos));
            break;
        }

        // Add the substring between startPos and foundPos
        substrings.push_back(origin.substr(startPos, foundPos - startPos));

        // Update startPos for the next search
        startPos = foundPos + 1;
    }

    return substrings;
}

std::vector<std::string> StringUtil::SplitString(const std::string &str,
                                                 const std::string &delimiter)
{
    std::vector<std::string> tokens;
    std::string s = str;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s); // 마지막 토큰 추가
    return tokens;
}

void StringUtil::ReplaceStrOpt(char *dest,
                               char const *const src,
                               char const *const pattern,
                               char const *const replace)
{
    /*
    It replaces all occurrences of a specific pattern (pattern) in a source string (src) 
    with a replacement string (replace) 
    and optionally writes the modified string to a destination buffer (dest).
    */
    size_t const replen = strlen(replace);
    size_t const patlen = strlen(pattern);

    size_t patcnt = 0;
    const char *oriptr;
    const char *patloc;

    // find how many times the pattern occurs in the original string
    for (oriptr = src; (patloc = strstr(oriptr, pattern));
         oriptr = patloc + patlen)
    {
        patcnt++;
    }

    {
        if (dest != NULL)
        {
            // copy the original string,
            // replacing all the instances of the pattern
            char *retptr = dest;
            for (oriptr = src; (patloc = strstr(oriptr, pattern));
                 oriptr = patloc + patlen)
            {
                size_t const skplen = patloc - oriptr;
                // copy the section until the occurence of the pattern
                strncpy(retptr, oriptr, skplen);
                retptr += skplen;
                // copy the replacement
                strncpy(retptr, replace, replen);
                retptr += replen;
            }
            // copy the rest of the string.
            strcpy(retptr, oriptr);
        }
    }
}

std::string StringUtil::TrimStart(const std::string &str, char c)
{
    size_t start = str.find_first_not_of(c);
    if (start == std::string::npos)
    {
        return ""; // Entire string consists of 'c'
    }
    return str.substr(start);
}

std::string StringUtil::TrimEnd(const std::string &str, char c)
{
    size_t end = str.find_last_not_of(c);
    if (end == std::string::npos)
    {
        return ""; // Entire string consists of 'c'
    }
    return str.substr(0, end + 1);
}

std::string StringUtil::Trim(const std::string &str, char c)
{
    return pr_trim_end(pr_trim_start(str, c), c);
}

std::wstring StringUtil::TrimStart(const std::wstring &str, char c)
{
    size_t start = str.find_first_not_of(c);
    if (start == std::wstring::npos)
    {
        return L""; // Entire string consists of 'c'
    }
    return str.substr(start);
}

std::wstring StringUtil::TrimEnd(const std::wstring &str, char c)
{
    size_t end = str.find_last_not_of(c);
    if (end == std::wstring::npos)
    {
        return L""; // Entire string consists of 'c'
    }
    return str.substr(0, end + 1);
}

std::wstring StringUtil::Trim(const std::wstring &str, char c)
{
    return pr_trim_end(pr_trim_start(str, c), c);
}
} // namespace Utils
