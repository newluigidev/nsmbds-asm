#include "ostream.hpp"

#include <cstring>

asm(R"(
.global _ZN7ostream5flushEv
.global _ZN7ostream5flushEy
.global _ZN7ostream5flushEmmm
.global _ZN7ostream5flushEmmmm
.global _ZN7ostream6bufferE

_ZN7ostream5flushEv:
_ZN7ostream5flushEy:
_ZN7ostream5flushEmmm:
_ZN7ostream5flushEmmmm:
	mov   r12,r12
	b     skip
	.word 0x00006464
_ZN7ostream6bufferE:
	.fill 120
	.word 0
skip:
	bx    lr
)");

ostream cout;

void ostream::set_buffer(const char* char_ptr)
{
	for (unsigned i = 0; i < buffer_size; i++)
	{
		if ((buffer[i] = char_ptr[i]) == '\0')
			break;
	}
}

#ifdef FXTYPE_INCLUDED

static void strrev(char* str)
{
	size_t len = strlen(str);
	for (size_t i = 0, j = len - 1; i < j; i++, j--)
	{
		char tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}
}

// Converts an int to a string and returns the string length
template<typename _Int>
static int int_to_str(_Int value, char* str)
{
	if (value == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		return 1;
	}

	size_t spos = 0;
	bool neg = value < 0;
	if (neg) value = -value;

	for (; value != 0; value /= 10)
		str[spos++] = '0' + value % 10;

	if (neg) str[spos++] = '-';

	str[spos] = '\0';
	strrev(str);
	
	return spos;
}

/*
	To get the whole part just shift 12 bits to the right.

	To get the fractional part, let's take 0x1842 for example:

	0x1842 / 0x1000 = 1.51611328125 @ Calculated with floats as reference
	0x842 * 10 = 0x5[294]
	0x294 * 10 = 0x1[9C8]
	0x9C8 * 10 = 0x6[1D0]
	               ^
	This is the fractional part.

	num: the fixed point number to convert
	decs: how many decimals to display (-1 = minimum)
	dec_bits: how many bits the fixed point uses for decimals
	str: the the pointer to the output string buffer
*/
template<typename _Int>
static void fx_to_str(_Int num, size_t decs, size_t dec_bits, char* str)
{
	typedef typename std::make_unsigned<_Int>::type _UInt;

	size_t spos = 0;

	_Int one = 1LL << dec_bits;
	_UInt dec_mask = one - 1;

	_Int whole = num / one;
	_UInt fract = ((num < 0) ? -num : num) & dec_mask;

	spos += int_to_str(whole, &str[spos]);
	if (decs == 0 || (decs == -1 && fract == 0))
	{
		str[spos] = '\0';
		return;
	}

	str[spos++] = '.';

	// Parse the fractionary part
	size_t fcount = 0;
	_UInt fnext = fract;
	while (fnext != 0 && fcount != decs)
	{
		_UInt fnum = fnext * 10;
		_UInt fwhole = fnum >> dec_bits;
		fnext = fnum & dec_mask;
		str[spos++] = '0' + fwhole;
		fcount++;
	}

	if (decs != -1 && fcount < decs)
	{
		for (size_t i = fcount; i < decs; i++)
			str[spos++] = '0';
	}

	str[spos] = '\0';
}

ostream& ostream::operator<<(const Fx16& fx)
{
	char buf[16];
	fx_to_str<s32>(fx, -1, 12, buf);
	return *this << (const char*)buf;
}

ostream& ostream::operator<<(const Fx32& fx)
{
	char buf[21];
	fx_to_str<s32>(fx, -1, 12, buf);
	return *this << (const char*)buf;
}

ostream& ostream::operator<<(const Fx64& fx)
{
	char buf[31];
	fx_to_str<s64>(fx, -1, 12, buf);
	return *this << (const char*)buf;
}

ostream& ostream::operator<<(const Fx64c& fx)
{
	char buf[45];
	fx_to_str<s64>(fx, -1, 32, buf);
	return *this << (const char*)buf;
}

#endif // FXTYPE_INCLUDED
