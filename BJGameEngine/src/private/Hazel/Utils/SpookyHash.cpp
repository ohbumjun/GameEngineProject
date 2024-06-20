#include "SpookyHash.h"

//
// short hash ... it could be used on any message,
// but it's used by Spooky just for short messages.
//

#define ALLOW_UNALIGNED_READS 1
void SpookyHash::Short(const void *message,
                       size_t length,
                       uint64 *hash1,
                       uint64 *hash2)
{
    uint64 buf[2 * NumVars];
    union
    {
        const uint8 *p8;
        uint32 *p32;
        uint64 *p64;
        size_t i;
    } u;

    u.p8 = (const uint8 *)message;

    if (!ALLOW_UNALIGNED_READS && (u.i & 0x7))
    {
        memcpy(buf, message, length);
        u.p64 = buf;
    }

    size_t remainder = length % 32;
    uint64 a = *hash1;
    uint64 b = *hash2;
    uint64 c = ConstValue;
    uint64 d = ConstValue;

    if (length > 15)
    {
        const uint64 *end = u.p64 + (length / 32) * 4;

        // handle all complete sets of 32 bytes
        for (; u.p64 < end; u.p64 += 4)
        {
            c += u.p64[0];
            d += u.p64[1];
            ShortMix(a, b, c, d);
            a += u.p64[2];
            b += u.p64[3];
        }

        //Handle the case of 16+ remaining bytes.
        if (remainder >= 16)
        {
            c += u.p64[0];
            d += u.p64[1];
            ShortMix(a, b, c, d);
            u.p64 += 2;
            remainder -= 16;
        }
    }

    // Handle the last 0..15 bytes, and its length
    d += ((uint64)length) << 56;
    switch (remainder)
    {
    case 15:
        d += ((uint64)u.p8[14]) << 48;
    case 14:
        d += ((uint64)u.p8[13]) << 40;
    case 13:
        d += ((uint64)u.p8[12]) << 32;
    case 12:
        d += u.p32[2];
        c += u.p64[0];
        break;
    case 11:
        d += ((uint64)u.p8[10]) << 16;
    case 10:
        d += ((uint64)u.p8[9]) << 8;
    case 9:
        d += (uint64)u.p8[8];
    case 8:
        c += u.p64[0];
        break;
    case 7:
        c += ((uint64)u.p8[6]) << 48;
    case 6:
        c += ((uint64)u.p8[5]) << 40;
    case 5:
        c += ((uint64)u.p8[4]) << 32;
    case 4:
        c += u.p32[0];
        break;
    case 3:
        c += ((uint64)u.p8[2]) << 16;
    case 2:
        c += ((uint64)u.p8[1]) << 8;
    case 1:
        c += (uint64)u.p8[0];
        break;
    case 0:
        c += ConstValue;
        d += ConstValue;
    }
    ShortEnd(a, b, c, d);
    *hash1 = a;
    *hash2 = b;
}


// do the whole hash in one call
void SpookyHash::Hash128(const void *message,
                         size_t length,
                         uint64 *hash1,
                         uint64 *hash2)
{
    if (length < BufferSize)
    {
        Short(message, length, hash1, hash2);
        return;
    }

    uint64 h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11;
    uint64 buf[NumVars];
    uint64 *end;
    union
    {
        const uint8 *p8;
        uint64 *p64;
        size_t i;
    } u;
    size_t remainder;

    h0 = h3 = h6 = h9 = *hash1;
    h1 = h4 = h7 = h10 = *hash2;
    h2 = h5 = h8 = h11 = ConstValue;

    u.p8 = (const uint8 *)message;
    end = u.p64 + (length / BlockSize) * NumVars;

    // handle all whole BlockSize blocks of bytes
    if (ALLOW_UNALIGNED_READS || ((u.i & 0x7) == 0))
    {
        while (u.p64 < end)
        {
            Mix(u.p64, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
            u.p64 += NumVars;
        }
    }
    else
    {
        while (u.p64 < end)
        {
            memcpy(buf, u.p64, BlockSize);
            Mix(buf, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
            u.p64 += NumVars;
        }
    }

    // handle the last partial block of BlockSize bytes
    remainder = (length - ((const uint8 *)end - (const uint8 *)message));
    memcpy(buf, end, remainder);
    memset(((uint8 *)buf) + remainder, 0, BlockSize - remainder);
    ((uint8 *)buf)[BlockSize - 1] = remainder;

    // do some final mixing
    End(buf, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
    *hash1 = h0;
    *hash2 = h1;
}


// init spooky state
void SpookyHash::Init(uint64 seed1, uint64 seed2)
{
    _length = 0;
    _remainder = 0;
    _state[0] = seed1;
    _state[1] = seed2;
}


// add a message fragment to the state
void SpookyHash::Update(const void *message, size_t length)
{
    uint64 h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11;
    size_t newLength = length + _remainder;
    uint8 remainder;
    union
    {
        const uint8 *p8;
        uint64 *p64;
        size_t i;
    } u;
    const uint64 *end;

    // Is this message fragment too short?  If it is, stuff it away.
    if (newLength < BufferSize)
    {
        memcpy(&((uint8 *)_data)[_remainder], message, length);
        _length = length + _length;
        _remainder = (uint8)newLength;
        return;
    }

    // init the variables
    if (_length < BufferSize)
    {
        h0 = h3 = h6 = h9 = _state[0];
        h1 = h4 = h7 = h10 = _state[1];
        h2 = h5 = h8 = h11 = ConstValue;
    }
    else
    {
        h0 = _state[0];
        h1 = _state[1];
        h2 = _state[2];
        h3 = _state[3];
        h4 = _state[4];
        h5 = _state[5];
        h6 = _state[6];
        h7 = _state[7];
        h8 = _state[8];
        h9 = _state[9];
        h10 = _state[10];
        h11 = _state[11];
    }
    _length = length + _length;

    // if we've got anything stuffed away, use it now
    if (_remainder)
    {
        uint8 prefix = BufferSize - _remainder;
        memcpy(&(((uint8 *)_data)[_remainder]), message, prefix);
        u.p64 = _data;
        Mix(u.p64, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
        Mix(&u.p64[NumVars], h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
        u.p8 = ((const uint8 *)message) + prefix;
        length -= prefix;
    }
    else
    {
        u.p8 = (const uint8 *)message;
    }

    // handle all whole blocks of BlockSize bytes
    end = u.p64 + (length / BlockSize) * NumVars;
    remainder = (uint8)(length - ((const uint8 *)end - u.p8));
    if (ALLOW_UNALIGNED_READS || (u.i & 0x7) == 0)
    {
        while (u.p64 < end)
        {
            Mix(u.p64, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
            u.p64 += NumVars;
        }
    }
    else
    {
        while (u.p64 < end)
        {
            memcpy(_data, u.p8, BlockSize);
            Mix(_data, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
            u.p64 += NumVars;
        }
    }

    // stuff away the last few bytes
    _remainder = remainder;
    memcpy(_data, end, remainder);

    // stuff away the variables
    _state[0] = h0;
    _state[1] = h1;
    _state[2] = h2;
    _state[3] = h3;
    _state[4] = h4;
    _state[5] = h5;
    _state[6] = h6;
    _state[7] = h7;
    _state[8] = h8;
    _state[9] = h9;
    _state[10] = h10;
    _state[11] = h11;
}


// report the hash for the concatenation of all message fragments so far
void SpookyHash::Final(uint64 *hash1, uint64 *hash2)
{
    // init the variables
    if (_length < BufferSize)
    {
        *hash1 = _state[0];
        *hash2 = _state[1];
        Short(_data, _length, hash1, hash2);
        return;
    }

    const uint64 *data = (const uint64 *)_data;
    uint8 remainder = _remainder;

    uint64 h0 = _state[0];
    uint64 h1 = _state[1];
    uint64 h2 = _state[2];
    uint64 h3 = _state[3];
    uint64 h4 = _state[4];
    uint64 h5 = _state[5];
    uint64 h6 = _state[6];
    uint64 h7 = _state[7];
    uint64 h8 = _state[8];
    uint64 h9 = _state[9];
    uint64 h10 = _state[10];
    uint64 h11 = _state[11];

    if (remainder >= BlockSize)
    {
        // _data can contain two blocks; handle any whole first block
        Mix(data, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
        data += NumVars;
        remainder -= BlockSize;
    }

    // mix in the last partial block, and the length mod BlockSize
    memset(&((uint8 *)data)[remainder], 0, (BlockSize - remainder));

    ((uint8 *)data)[BlockSize - 1] = remainder;

    // do some final mixing
    End(data, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);

    *hash1 = h0;
    *hash2 = h1;
}
