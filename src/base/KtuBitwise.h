#pragma once
#include <type_traits>
#include <limits>
#include <assert.h>


template<typename INT>
class KtuBitwise
{
	static_assert(std::is_integral<INT>::value, "KtuBitwise only support integral type");  

public:
	constexpr static const int kCharBits = 8;
	constexpr static const int kIntBits = sizeof(INT) * kCharBits;
	constexpr static const INT kAllZero = 0;
	constexpr static const INT kAllOnes = ~INT(0);

	// Determines if the number is odd.
	static bool isOdd(INT x) { return x & 1;  }

	// Determines if the number is even.
	static bool isEven(INT x) { return !isOdd(x); }

	// 将x的第n位置1，最低位的序号为1，下同
	static void set(INT& x, int n) {
		assert(n <= kIntBits);
		x |= 1 << n;
	}

	// 将x的第n位清零
	static void clear(INT& x, int n) {
		assert(n <= kIntBits);
		x &= !(1 << n);
	}

	// 将x的第n位反转
	static void flip(INT& x, int n) {
		assert(n <= kIntBits);
		x ^= (1 << (n - 1));
	}

	// x的第n位是否置1
	static bool isSet(INT x, int n) {
		assert(n <= kIntBits);
		return (x >> (n - 1)) & 1;
	}



	// 清零x的最低有效位
	static INT lsbClear(INT x) { return x & (x - 1); }

	// leave only lowest bit
	static INT lsbOnly(INT x) { return x & -x; }


	// reverse right nth bits of x
	static INT revbin(INT x, unsigned n);

	// 判断n是否为2的幂
	static bool isPower2(INT n) {
		return n != 0 && lsbClear(n) == 0;
	}

	// 判断n是否为4的幂
	static bool isPower4(INT n);

	// Determines if the number is prime.
	static bool isPrime(INT n);

	// 返回n对齐到align的值
	static INT align(INT n, INT alignTo) {
		return (n + align - 1) & (~(align - 1));
	}

	/** Factorial of a number. */
	static INT factorial(INT n) { 
		return (n <= 1) ? 1 : n * factorial(n - 1); // return n!
	}

	//! Integer Power function
	//! The function is overflow safe, all the calculations are checked to make sure that the result won't overflow.   Works for positive or negative bases.
	//! Any overflow will result in the return value of zero
	//! \param base
	//! \param exponent
	//! \return  zero if underflow or overflow occurs.
	static INT power(INT base, INT exponent);


	/* Compute the absolute value of X as an unsigned type.  */
	static INT abs(INT x) {
		return x >= 0 ? x : -x;
	}


	// 整型的快速交换
	static void swap(INT& a, INT& b) {
		a ^= b, b ^= a, a ^= b;
	}

	// calculate number of bits set via lookup table
	static int count(INT x);


	// computes the index of the most-significant bit
	//
	// Examples:

	// 十六进制例子
	//  0x00000000  :   0
	//  0x00000001  :   1
	//  0x00000002  :   2
	//  ...
	//  0x00010000  :   17
	//  0x00020000  :   18
	//  0x00040000  :   19
	//  ...
	//  0x80000000  :   32

	// 二进制例子
	//  0   0000 0000   :   0
	//  1   0000 0001   :   1
	//  2   0000 0010   :   2
	//  3   0000 0011   :   2
	//  4   0000 0100   :   3
	//  ...
	//  126 0111 1110   :   7
	//  127 0111 1111   :   7
	//  128 1000 0000   :   8
	//  129 1000 0001   :   8
	//  ...
	//  253 1111 1101   :   8
	//  254 1111 1110   :   8
	//  255 1111 1111   :   8
	static int msbIndex(INT x); 

	//Find lowest lsb 
	static int lsbIndex(INT x);


	static INT gcd(INT m, INT n);

	/// Returns the least common multiple of two integers. Will crash unless the inputs are positive.
	static INT lcm(INT m, INT n);

	//// 下列实现参考gcc的hwint.c(HostWideInt)

	/* Given X, an unsigned number, return the largest int Y such that 2**Y <= X.
	   If X is 0, return -1.  */
	static int floorLog2(INT x);

	/* Given X, an unsigned number, return the largest Y such that 2**Y >= X.  */
	static int ceilLog2(INT x);

	/* Return the logarithm of X, base 2, considering X unsigned,
	   if X is a power of 2.  Otherwise, returns -1.  */
	static int exactLog2(INT x);

	/** Returns the closest power-of-two number less or equal to x.
		@note 0 and 1 are powers of two, so FirstPO2From(0)==0 and FirstPO2From(1)==1.
	*/
	static INT floorPower2(INT x);

	/** Returns the closest power-of-two number greater or equal to x.
		@note 0 and 1 are powers of two, so FirstPO2From(0)==0 and FirstPO2From(1)==1.
	*/
	static INT ceilPower2(INT x);

	//// -------------------------------------------------------------


	/* Return the number of set bits in X.  */
	static int popCount(INT x); // popcount_hwi

	// count leading zeros in an integer
	static int clz(INT x) {
		return kIntBits - msbIndex(x);
	}

	// count trailing zeros in an integer
	static int ctz(INT x) {
		return x ? floorLog2(lsbOnly(x)) : kIntBits;
	}

	/* Similar to ctz, except that the least significant bit is numbered
	starting from 1, and X == 0 yields 0.  */
	static int ffs(INT x) {
		return 1 + floorLog2(lsbOnly(x));
	}

	//// -------------------------------------------------------------


	//!Returns integer's higher bits from position up to msb
	//!Lower bits are set to zero
	static INT maskLeft(INT val, int pos);


	//!Returns integer's lower bits from position down to lsb
	//!Higher bits are set to zero
	static INT maskRight(INT val, int pos);


	//!Rotate Left with carry over
	static INT rol(INT val, int shift_register) {
		return (val << shift_register) | (val >> (kIntBits - shift_register));
	}

	//!Rotate Right with carry over
	static INT ror(INT val, int shift_register) {
		return (val >> shift_register) | (val << (kIntBits - shift_register));
	}

	//!Left Shift with shift off
	//!Shifts the value to the left by n bits.
	//! The bits that would get shifted off value are returned in lsb
	static INT lso(INT& val, int n);

	//!Right Shift with shift off
	//!Shifts the value to the right by n bits.
	//! The bits that would get shifted off value are returned in msb
	static INT rso(INT& val, int n);


	//!Left Shift with shift off
	//!Shifts the value to the left by n bits.
	//! The bits that get shifted off val are returned in lsb of return value
	//!\param [in,out] val  Value to shift
	//!\param [in] old_val Values to shift on to val
	//!\returns value with bits that are shifted off val in lsb
	static INT lsoso(INT& val, INT& old_val, int n);

	//!Right Shift with shift off
	//!Shifts the value to the right by n bits.
	//! The bits that would get shifted off value are returned in msb of return value
	//!\param [in,out] val  Value to shift
	//!\param [in] old_val Values to shift on to val
	static INT rsoso(INT& val, INT& old_val, int n);
};

#pragma warning(disable: 4146) // warning C4146: unary minus operator applied to unsigned type, result still unsigned

// reverse right nth bits of x
template<typename INT>
INT KtuBitwise<INT>::revbin(INT x, unsigned n)
{
	int r(0);

	while(n-- > 0)
	{
		r <<= 1;
		r += x & 1;
		x >>= 1;
	}

	return r;
}


// 判断n是否为4的幂
template<typename INT>
bool KtuBitwise<INT>::isPower4(INT n)
{
	int nMeets(0);
	for(unsigned int i = 0; i < kIntBits; i++) {
		if((1 & n) && ((++nMeets > 1) || (1 & i))) // 只能偶数位为非零(zero based)
			return false;

		n >>= 1;
	}

	return true;
}

// determine if number is prime (slow, simple method)
// https://en.ikipedia.org/wiki/Primality_test#Pseudocode
// (thanks to K. Rosenberg for the tip)
template<typename INT>
bool KtuBitwise<INT>::isPrime(INT n)
{
	// check base cases (0, 1, 2, 3, divisible by 2, divisible by 3)
	if (n <= 1)  return false;
	else if (n <= 3)  return true;
	else if (!(n & 1)) return false; // divisible by 2
	else if (!(n % 3)) return false; // divisible by 3

	unsigned int r = 5;
	while (r*r <= n) {
		if ((n % r) == 0 || (n % (r + 2)) == 0)
			return false;
		r += 6;
	}
	return true;
}



template<typename INT>
INT KtuBitwise<INT>::power(INT base, INT exponent)
{
	bool negative(false);
	INT max = std::numeric_limits<INT>::max();
	if (base<0) {
		base = -base;
		if (isOdd(exponent)) 
			negative = true;
	}

	INT result = 1;
	while (exponent) {
		if (exponent & 1) {
			if (result > max / base) {  //Determine if value of result*base > INT_MAX;
				return 0;
			}

			result *= base;
		}

		exponent >>= 1;

		if (exponent != 0) {
			if (base > max / base) { //Determine if value of base*base > INT_MAX;
				return 0;
			}
			base *= base;
		}
	}

	return negative ? -result : result;
}

//// 下列函数实现参考gcc的hwint.c(HostWideInt)

/* Given X, an unsigned number, return the largest int Y such that 2**Y <= X.
	If X is 0, return -1.  */
template<typename INT>
int KtuBitwise<INT>::floorLog2(INT x)
{
	int t = 0;

	if(x == 0) return -1;

	if(sizeof(x) > 8)
		if(x >= (INT) 1 << (t + 64)) t += 64;
	if(sizeof(x) > 4)
		if(x >= ((INT) 1) << (t + 32)) t += 32;
	if(x >= ((INT) 1) << (t + 16)) t += 16;
	if(x >= ((INT) 1) << (t + 8)) t += 8;
	if(x >= ((INT) 1) << (t + 4)) t += 4;
	if(x >= ((INT) 1) << (t + 2)) t += 2;
	if(x >= ((INT) 1) << (t + 1)) t += 1;

	return t;
}

/* Given X, an unsigned number, return the largest Y such that 2**Y >= X.  */
template<typename INT>
int KtuBitwise<INT>::ceilLog2(INT x)
{
	return floorLog2(x - 1) + 1;
}

/* Return the logarithm of X, base 2, considering X unsigned,
	if X is a power of 2.  Otherwise, returns -1.  */
template<typename INT>
int KtuBitwise<INT>::exactLog2(INT x)
{
	if(x != lsbOnly(x)) return -1;
	return floorLog2(x);
}

template<typename INT>
INT KtuBitwise<INT>::floorPower2(INT x)
{
	if(x == lsbOnly(x)) return x;
	return ceilPower2(x) / 2;
}

template<typename INT>
INT KtuBitwise<INT>::ceilPower2(INT x)
{
	if (isPower2(x))
		return x;

//#if sizeof(INT) == 4
	--x;            
	x |= x >> 16;
	x |= x >> 8;
	x |= x >> 4;
	x |= x >> 2;
	x |= x >> 1;
	++x;
	return x;
//#else
//	INT r(1);
//	while(r < x) r *= 2;
//	return r;
//#endif
}


/* Return the number of set bits in X.  */
template<typename INT>
int KtuBitwise<INT>::popCount(INT x) 
{
	int ret = 0;

	for(size_t i = 0; i < kIntBits; i += 1) {
		ret += x & 1;
		x >>= 1;
	}

	return ret;
}


template<typename INT>
int KtuBitwise<INT>::count(INT x)
{
	// a lookup table for the number of bits set in a char
	///@cond
	static const int NUMSET_LOOKUP[256] =
	{
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
	};
	///@endcond

	unsigned char *ptr = (unsigned char *)&x;
	int ret = 0;
	for (size_t i = 0; i < sizeof(INT); i++)
		ret += NUMSET_LOOKUP[*ptr++];

	return ret;
}


template<typename INT>
int KtuBitwise<INT>::msbIndex(INT x)
{
#if 0
	// slow method; look one bit at a time
	int bits;
	for (bits = 0; x != 0 && bits < 32; x >>= 1, ++bits)
		;
	return bits;
#endif

	// number of leading zeros in a byte
	//  0   0000 0000   :   8
	//  1   0000 0001   :   7
	//  2   0000 0010   :   6
	//  3   0000 0011   :   6
	//  4   0000 0100   :   5
	//  ...
	//  126 0111 1110   :   1
	//  127 0111 1111   :   1
	//  128 1000 0000   :   0
	//  129 1000 0001   :   0
	//  ...
	//  253 1111 1101   :   0
	//  254 1111 1110   :   0
	//  255 1111 1111   :   0
	const int leading_zeros[256] = {
		8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	// look for first non-zero byte
	int bits = kIntBits;
	for (unsigned int i = bits; i>0; i -= kCharBits) {
		unsigned int b = (x >> (i - kCharBits)) & 0xff;
		if (b)
			return bits - leading_zeros[b];
		else
			bits -= kCharBits;
	}
	return 0;
}


template<typename INT>
INT KtuBitwise<INT>::gcd(INT m, INT n)
{
	// could use compile-time assertion
	// but involves messing with complex template stuff.
	assert(!(m == 0 && n == 0));

	while(1) {
		m %= n;
		if (m == 0) return (n > 0 ? n : -n);
		n %= m;
		if (n == 0) return (m > 0 ? m : -m);
	}
}


template<typename INT>
INT KtuBitwise<INT>::lcm(INT m, INT n)
{
	assert(m > 0 && n > 0);

	INT gcd = gcd(m, n);
	return gcd * (m/gcd) * (n/gcd);
}


template<typename INT>
INT KtuBitwise<INT>::maskLeft(INT val, int pos) 
{
	if (pos >= kIntBits)
		return 0;

	return val & ~((INT(1) << pos) - 1);
}


template<typename INT>
INT KtuBitwise<INT>::maskRight(INT val, int pos)
{
	int bits_minus_one = sizeof(INT) * kCharBits - 1;
	if (pos > bits_minus_one)
		return 0;
	else if (pos == bits_minus_one)
		return val & (INT(1) << bits_minus_one);
	else
		return val & ((INT(1) << (pos + 1)) - 1);
}


template<typename INT>
int KtuBitwise<INT>::lsbIndex(INT x)
{
	assert(sizeof(x) == 4); // TODO: 仅支持32位整数

	static const unsigned int multiplyDeBruijnBitPosition[32] =
	{
		// precomputed lookup table
		0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
		31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9
	};

	x  = lsbOnly(x);

	// DeBruijn constant
	size_t ux = size_t(x) * 0x077CB531;
	// get upper 5 bits
	ux >>= 27;
	// convert to actual position
	return multiplyDeBruijnBitPosition[ux];
}


template<typename INT>
INT KtuBitwise<INT>::lso(INT& val, int n)
{
	INT temp(val);
	if (n >= kIntBits)
		val = 0;
	else 
		val <<= n;

	n %= 32; // TODO: TEST
	return temp >> (kIntBits - n);
}


template<typename INT>
INT KtuBitwise<INT>::rso(INT& val, int n)
{
	INT temp(val);
	if (n >= kIntBits) 
		val = 0;
	else 
		val >>= n;

	n %= 32; // TODO: TEST??
	return temp << (kIntBits - n);
}


template<typename INT>
INT KtuBitwise<INT>::lsoso(INT& val, INT& old_val, int n)
{
	INT temp(val);
	val = (val << n) | old_val;
	return temp >> (kIntBits - n);
}


template<typename INT>
INT KtuBitwise<INT>::rsoso(INT& val, INT& old_val, int n)
{
	INT temp(val);
	val = (val >> n) | old_val;
	return temp << (kIntBits - n);
}