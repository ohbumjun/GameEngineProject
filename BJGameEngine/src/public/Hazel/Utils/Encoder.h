#pragma once
#include <stdlib.h>

namespace Utils
{

/*------ Base64 encoding Table ------*/
static const char MimeBase64[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

/*------ Base64 Decoding Table ------*/
static int DecodeMimeBase64[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

/*
인코딩 기본 원리
- 8 비트 x 3 글자 ==> 6 비트 x 4 글자

*/
// text : 인코딩할 이진 데이터가 들어있는 문자열 포인터
// numBytes : 데이터 크기
// encodedText : 결과를 담을 변수 주소
class Encoder
{
public :
    static size_t calculateCharArraySize(char* array) {
        size_t count = 0;
        while (array[count] != '\0') {
            count++;
        }
        return count;
    }

    static int base64_encode_length(char* text, int numBytes)
    {
        int   size;
        size = (4 * (numBytes / 3)) + (numBytes % 3 ? 4 : 0) + 1;
        return size;
    }
    static int base64_encode(char* text, int numBytes, char** encodedText)
    {
        char input[3] = { 0,0,0 };
        char output[4] = { 0,0,0,0 };
        int   index, i, j, size;
        char* p, * plen;
        plen = text + numBytes - 1;
        size = (4 * (numBytes / 3)) + (numBytes % 3 ? 4 : 0) + 1;
        (*encodedText) = (char*)malloc(size);
        j = 0;
        for (i = 0, p = text; p <= plen; i++, p++) {
            index = i % 3;
            input[index] = *p;

            // 3 개의 글자를 24 비트 버퍼에 담는다.
            // 이제 이것을 6 비트 * 4 형태로 변환할 것이다.
            // 3번째 글자를 다 입력했거나 / 문자열 끝에 도달했을 때
            if (index == 2 || p == plen) {
                // 상위 6비트 채우기
                // 0xFC : 1111 1100 
                // 1) input[0] 이라는 8 bit 중에서 상위 6개 bit 추출, 6 개 비트 형태로 만들기
                output[0] = ((input[0] & 0xFC) >> 2);

                // 0x3   : 0000 0011
                // 0xF0 : 1111 0000
                // 2) input[0] 의 하위 2 비트 + input[1]의 상위 4비트 == 6 bit
                output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);

                // 0xF   :  0000 1111
                // 0xC0 :  1100 0000
                // 3) input[1] 의 하위 4비트 + input[2] 의 상위 2비트 == 6 bit
                output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);

                // 0x3F : 0011 1111
                // 4) input[2] 의 하위 6 비트 추출 == 6 비트
                output[3] = (input[2] & 0x3F);

                (*encodedText)[j++] = MimeBase64[output[0]];
                (*encodedText)[j++] = MimeBase64[output[1]];

                // index == 0 : 문자 하나만 입력했을 경우
                (*encodedText)[j++] = index == 0 ? '=' : MimeBase64[output[2]];

                // index < 2   : 3번째 문자가 입력되지 않은 경우
                (*encodedText)[j++] = index < 2 ? '=' : MimeBase64[output[3]];

                // 다시 reset
                input[0] = input[1] = input[2] = 0;
            }
        }
        (*encodedText)[j] = '\0';
        return size;
    }

    /*
    디코딩 원리
    - 인코딩된 문자열을 Table 을 이용하여, 해당 값의 2진법으로 표시
    - 24 비트 버퍼에 가장 위쪽 비트부터 6비트씩 4개 값 넣기
    - 8비트로 잘라, 그 값을 ASCII 로 읽는다.
    - 만약 '=' 로 패딩되었다면 제외
    - ASCII 를 다시 문자열로 읽어 출력
    */
    static int base64_decode(char* encodedText, int numBytes, char* decodedText)
    {
        const char* cp;
        int space_idx = 0, phase;
        int d, prev_d = 0;
        unsigned char c;
        space_idx = 0;
        phase = 0;
        for (cp = encodedText; *cp != '\0'; ++cp) {
            // 문자열 하나를 해당값의 2진법으로 표현
            d = DecodeMimeBase64[(int)*cp];

            // 24비트 공간에 6비트씩 4개 값 넣기
            if (d != -1) {
                switch (phase) {
                case 0:
                    ++phase;
                    break;
                case 1:
                    // 1) 이전 6비트 왼쪽으로 2비트 몰기 + 현재 6비트 중에서 상위 2비트 추출해서 하위 2비트로 만들기 == 8 bit
                    c = ((prev_d << 2) | ((d & 0x30) >> 4));
                    if (space_idx < numBytes)
                        decodedText[space_idx++] = c;
                    ++phase;
                    break;
                case 2:
                    // 2) 이전 6비트 중, 하위 4비트 추출해서, 상위 4비트로 만들기
                    //    + 현재 6비트 중 상위 4비트 뽑기 == bit
                    c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
                    if (space_idx < numBytes)
                        decodedText[space_idx++] = c;
                    ++phase;
                    break;
                case 3:
                    // 3) 이전 6비트 중 하위 2비트 + 현재 6비트
                    c = (((prev_d & 0x03) << 6) | d);
                    if (space_idx < numBytes)
                        decodedText[space_idx++] = c;
                    phase = 0;
                    break;
                }
                prev_d = d;
            }
        }
        return space_idx;
    }

private :
 
};

}


