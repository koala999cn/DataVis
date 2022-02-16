#pragma once
#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>


template<typename KREAL>
class KtuMath
{
public:

    /// 一些数学常量
    static constexpr KREAL pi = static_cast<KREAL>(3.1415926535897932384626433832795028841971693993751L);
    static constexpr KREAL e = static_cast<KREAL>(2.7182818284590452353602874713526624977572L);
    static constexpr KREAL golden_section = static_cast<KREAL>(0.6180339887498948482045868343656381177203L); // sqrt(5)/2-0.5
    static constexpr KREAL nan = std::numeric_limits<KREAL>::quiet_NaN();
    static constexpr KREAL inf = std::numeric_limits<KREAL>::infinity();
    static constexpr KREAL neginf = -inf;
    static constexpr KREAL eps = std::numeric_limits<KREAL>::epsilon();


    /*************** SCALAR ALGORITHM *****************/

    static bool isUndefined(KREAL x) { return std::isnan(x) || std::isinf(x); }

    static bool isDefined(KREAL x) { return std::isfinite(x); }

    static bool containUndefined(const KREAL x[], unsigned n);

    // x, 返回1; 小于0, 返回-1; 等于0, 返回0.
    static int sign(KREAL x) {
        return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
    }

    static int trunc(KREAL x) {
        return (x >= 0) ? static_cast<int>(x) : -static_cast<int>(-x);
    }

    static KREAL fract(KREAL x) { // Computes the fractional part of the input number.
        return x - std::floor<KREAL>(x);
    }


    static void killDenormal(KREAL& x); // 消除非正规化数值影响.若val为极小值，则置为0.

    // 近似等于比较，一个绝对值版本，一个相对值版本
    static bool almostEqual(KREAL x1, KREAL x2, KREAL tol = 1e-5);
    static bool almostEqualRel(KREAL x1, KREAL x2, KREAL rel_tol = 0.001);

    static KREAL clamp(KREAL x, KREAL low, KREAL high) { return std::max(std::min(x, high), low); } // 确保value在[low, high]之间


    // 基于base的对数值 --> 自然对数值
    // ll为基于base的对数值，base == 0表示ll为非对数值
    static KREAL logFromBase(KREAL ll, KREAL base) {
        return base == 0 ? std::log(ll) : ll * std::log(base);
    }

    // 自然对数值 --> 基于base的对数值
    // ll为自然对数值，base == 0表示将ll转换为非对数值
    static KREAL logToBase(KREAL ll, KREAL base) {
        return base == 0 ? std::exp(ll) : ll / std::log(base);
    }


    // x = log(a), y = log(b)
    // return log(a+b)
    static KREAL addLog(KREAL x, KREAL y);

    // x = log(a), y = log(b)
    // return log(a-b)
    static KREAL subLog(KREAL x, KREAL y);

    // sinc(x) = sin(pi*x) / (pi*x)
    static KREAL sinc(KREAL x);

    // logit function
    // Inverse of the sigmoidal logistic function
    // Also known as log-odds score or ratio
    static KREAL logit(KREAL x) { return std::log(x / (1 - x)); }
    static KREAL logistic(KREAL x) { return 1 / (1 + std::exp(-x)); }
    static KREAL logistic(const KREAL theta[], const KREAL x[], unsigned n) {
        return logistic(-dot(theta, x, n));
    }

    // Calculate the binomial coefficient
    // It is the coefficient of the x^k term in the polynomial expansion of the binomial power (1+x)^n
    // it is fiven by the binomial formula:
    // B(n, k) = factorial(n) / [factorial(k) * factorial(n-k)]
    static KREAL binomialCoeff(KREAL n, KREAL k);

    // Returns stirlings approximation of x!
    // x! ~= sqrt(2*pi*x)*(x/e)^x
    static KREAL factorial(KREAL x) { // 实数版本的阶乘，使用stirlings近似。
        return std::sqrt(2 * pi * x) * std::pow(x / e, x);
    }

    // 一个基于stirlings，但精度更高的近似
    // http://www.rskey.org/CMS/index.php/the-library/11
    static KREAL factorialX(KREAL x) {
        return factorial(x) * std::exp(1/(12*x+2/(5*x+53/(42*x))));
    }

    /*************** VECTOR ALGORITHM *****************/

    static void assign(KREAL x[], unsigned n, KREAL val);
    static void zeros(KREAL x[], unsigned n) { assign(x, n, 0); }
    static void ones(KREAL x[], unsigned n) { assign(x, n, 1); }

    static KREAL lerp(KREAL xmin, KREAL xmax, KREAL x/*[0, 1]*/) { return xmin + x*(xmax-xmin); } // 线性插值
    static KREAL quantile(const KREAL x[], unsigned n, KREAL factor);

    static KREAL sum(const KREAL x[], unsigned n); // 各元素之和
    static KREAL sumAbs(const KREAL x[], unsigned n); // 各元素绝对值之和
    static KREAL sum2(const KREAL x[], unsigned n); // 各元素平方之和
    static KREAL sumPower(const KREAL x[], unsigned n, KREAL power); // 各元素绝对值的power次方之和
    static KREAL norm(const KREAL x[], unsigned n, KREAL power); // power阶范式
    static void  normalize(const KREAL x[], unsigned n, KREAL power, KREAL newNorm); // 对x做power阶范式规范化处理

    // Convert the vector x to probilities.  Divide each indice by the sum of the vector
    static void prob(KREAL x[]/*inout*/, unsigned n);

    static KREAL product(const KREAL x[], unsigned n); // 各元素之积
    static KREAL mean(const KREAL x[], unsigned n); // sum(x[i]) / n
    static KREAL var(const KREAL x[], unsigned n, KREAL mean);
    static KREAL var(const KREAL x[], unsigned n) { return var(x, n, mean(x, n)); }
    static KREAL standardDeviation(const KREAL x[], unsigned n) { return std::sqrt(var(x, n)); }

    // Evaluate mean of a vector from elements that are not zeros
    static KREAL nonZeroMean(const KREAL x[], unsigned n);

    static void scale(KREAL x[], unsigned n, KREAL alpha); // x[i] *= alpha
    static void shift(KREAL x[], unsigned n, KREAL scalar); // x[i] += scalar
    static void subMean(KREAL x[], unsigned n); // x[i] -= mean

    static void add(const KREAL x[], const KREAL y[], KREAL r[], unsigned n); // r[i] = x[i] + y[i]
    static void sub(const KREAL x[], const KREAL y[], KREAL r[], unsigned n); // r[i] = x[i] - y[i]
    static void mul(const KREAL x[], const KREAL y[], KREAL r[], unsigned n); // r[i] = x[i] * y[i]

    static KREAL dot(const KREAL x[], const KREAL y[], unsigned n); // return Sum(x[i]*y[i])

    // 加权加法：r[i] = phase1 * x[i] + phase2 * y[i]
    void wadd(const KREAL x[], KREAL phase1, const KREAL y[], KREAL phase2, KREAL r[], unsigned n) {
        for (unsigned i = 0; i < n; i++)
            *r++ = phase1 * *x++ + phase2 * *y++;
    }


    /*! \fn std::vector<KREAL> combAdd(std::vector< REAL >& x, std::vector< REAL >& y)
    \brief Adds the x and y vector combinatorially in result
    \Example [ 1 4 ] + [ 2 7 ] = [ 3 6 8 11 ]
    \param x vector of (ints or doubles)
    \param y vector of (ints or doubles)
    \return the result of the two vectors added together
    */
    static std::vector<KREAL> combAdd(const std::vector<KREAL>& x, const std::vector<KREAL>& y);

    /*! \fn std::vector<KREAL> combMul(std::vector< REAL >& x, std::vector< REAL >& y)
    \brief Multiply the x and y vector combinatorially in result
    \param x vector of (ints or doubles)
    \param y vector of (ints or doubles)
    \return the result of the two vectors muled together
    */
    static std::vector<KREAL> combMul(const std::vector<KREAL>& x, const std::vector<KREAL>& y);

    // 过零数和过零率
    static unsigned getZeroCrossing(const KREAL x[], unsigned n);
    static KREAL getZcr(const KREAL x[], unsigned n) { return  KREAL(getZeroCrossing(x, n)) / (n - 1); }

    static void applyFloor(KREAL x[], unsigned n, KREAL floor); // X = max(X, floor)
    static void applyExp(KREAL x[], unsigned n); // X = exp(X)
    static void applyLog(KREAL x[], unsigned n); // X = log(X)


    // computes the gini impurity of a possibly unnormalized distribution
    // return 1.0 - Sum(x^2) / Sum(x)^2
    static KREAL gini(const KREAL x[], unsigned n); // 支持KREAL=int的情况

    // exponentiates and normalizes a vector
    static KREAL expAndNorm(KREAL x[], unsigned n);

    ////////////////////////////////////////////////////////////////////////

    // computes the entropy of a possibly unnormalized distribution
    static KREAL entropy(const KREAL x[], unsigned n); // 支持KREAL=int的情况

    //Shannon'tag Relative entropy (Kullback-Liebler Convergence)
    //Normalized for A->B and B->A
    static KREAL relEntropy(const KREAL x[], const KREAL y[], unsigned n);

    static KREAL crossEntropy(const KREAL x[], const KREAL y[], unsigned n);


    /*************** SEARCH ALGORITHM *****************/

    static KREAL min(const KREAL x[], unsigned n);
    static KREAL max(const KREAL x[], unsigned n);
    static std::pair<KREAL, KREAL> minmax(const KREAL x[], unsigned n);

    static unsigned argMin(const KREAL x[], unsigned n); // 返回最小值的索引[0, dim)
    static unsigned argMax(const KREAL x[], unsigned n); // 返回最大值的索引[0, dim)
    static unsigned argRand(const KREAL x[], unsigned n); // 返回累加和大于随机值的索引[-1, dim)
    static std::pair<unsigned, unsigned> argMixMax(const KREAL x[], unsigned n);


private:
    KtuMath() { }
    ~KtuMath() { }
};


// 参考Laurent de Soras. Denormal numbers in floating point signal processing applications.
template<class KREAL>
void KtuMath<KREAL>::killDenormal(KREAL& x)
{
    static constexpr KREAL antiDenormal = 1e-18;
    x += antiDenormal;
    x -= antiDenormal;
}

template<class KREAL>
bool KtuMath<KREAL>::almostEqual(KREAL x1, KREAL x2, KREAL tol)
{
    return std::abs(x1 - x2) <= tol;
}

template<class KREAL>
bool KtuMath<KREAL>::almostEqualRel(KREAL x1, KREAL x2, KREAL rel_tol)
{
    // a==b handles infinities.
    if(x1 == x2) return true;
    double diff = std::abs(x1 - x2);
    if(isUndefined(diff))
        return false;
    return diff <= rel_tol*(std::abs(x1) +std::abs(x2));
}

template<class KREAL>
KREAL KtuMath<KREAL>::addLog(KREAL x, KREAL y)
{
    if (x == neginf) {
        return y;
    }
    else if (y == neginf) {
        return x;
    }
    else if (x > y) {
        return x + log(1 + std::exp(y - x));
    }
    else {
        return y + log(1 + std::exp(x - y));
    }
}

template<class KREAL>
KREAL KtuMath<KREAL>::subLog(KREAL x, KREAL y)
{
    if (x < y) // log(-k)
        return nan;

    else if (x == y) // log(0)
        return neginf;

    else if (y == neginf) // log(x-0) = log(x) = x
        return x;

    else
        return x + log(1 - std::exp(y - x));
}


template<class KREAL>
KREAL KtuMath<KREAL>::sinc(KREAL x)
{
    KREAL _x = pi * x;

    // _x ~ 0 approximation
    // from : http://mathworld.wolfram.com/SincFunction.html
    // sinc(z) = \prod_{k=1}^{\infty} { cos(\pi z / 2^k) }
    if (abs(_x) < 0.01)
        return std::cos(_x / 2)*std::cos(_x / 4)*std::cos(_x / 8);

    return std::sin(_x) / _x;
}


template<class KREAL>
bool KtuMath<KREAL>::containUndefined(const KREAL x[], unsigned n)
{
    for(unsigned i = 0; i < n; i++) {
        if(isUndefined(*x++))
            return true;
    }

    return false;
}


// 数学定义为：E(X) = -Sum{ P(xi) * log(P(xi)) }
// 物理解释：根据Shannon信息论，给定一个字符集的概率分布，可以设计一种编码，
// 使得表示该字符集组成的字符串平均需要的比特数最少。假设这个字符集是X，
// 对于其中某个字符xi，其出现概率为P(xi)，那么其最优编码（哈夫曼编码）
// 平均需要的比特数等于这个字符集的熵。
// 以下算法为了处理非归一化情况，实际求解E(px/sum(px))
template<class KREAL>
KREAL KtuMath<KREAL>::entropy(const KREAL x[], unsigned n)
{
    KREAL Z = 0;
    KREAL H = 0;

    for(unsigned i = 0; i < n; i++) {
        if(x[i] > 0)
            H += x[i] * std::log(x[i]);
        Z += x[i];
    }

    return (std::log(Z) - H / Z) / std::log<KREAL>(2);
}

// KL散度用于计算两个随机变量的差异程度，又称为信息增益、相对熵。
// A关于B的KL散度数学定义为：
//   KL(A || B) = E(A) - Sum{ PA(xi) * log(PB(xi)) }
// 由上式可以看出，KL散度是不对称的，即A关于B的KL散度不等于B关于A的KL散度
// 以下算法没有严格使用KL散度的数学定义，而是返回0.5(KL(A||B) + KL(B||A))
template<class KREAL>
KREAL KtuMath<KREAL>::relEntropy(const KREAL x[], const KREAL y[], unsigned n)
{
    KREAL rel_entropy = 0;

    for (unsigned i = 0; i < n; i++)
        rel_entropy += (x[i] * std::log2(x[i] / y[i]) + y[i] * std::log2(y[i] / x[i])) / 2;

    return rel_entropy;
}

// 交叉熵的意义是：使用随机变量B的最优编码方式对随机变量A编码所需要的字符数。
//   具体来说就是使用哈夫曼编码却根据B的概率分布对A进行编码，所需要的编码数。
// 交叉熵 = KL散度 + 熵
template<class KREAL>
KREAL KtuMath<KREAL>::crossEntropy(const KREAL x[], const KREAL y[], unsigned n)
{
    KREAL cross_entropy = 0;

    for (unsigned i = 0; i < n; i++)
        cross_entropy += x[i] * std::log2(y[i]);

    return -cross_entropy;
}

template<class KREAL>
KREAL KtuMath<KREAL>::gini(const KREAL x[], unsigned n)
{
    KREAL Z = 0;
    KREAL G = 0;

    for(unsigned i = 0; i < n; i++) {
        G += x[i] * x[i];
        Z += x[i];
    }

    return 1.0 - G / (Z * Z);
}

template<class KREAL>
KREAL KtuMath<KREAL>::expAndNorm(KREAL x[], unsigned n)
{
    if(n == 0) return 0;

    KREAL maxValue = x[0];
    for(unsigned i = 1; i < n; i++)
        maxValue = std::max(maxValue, x[i]);

    KREAL Z = 0;
    for(unsigned i = 0; i < n; i++) {
        x[i] = std::exp(x[i] - maxValue);
        Z += x[i];
    }

    double *v_ptr = &x[0];
    for(unsigned i = n / 2; i != 0; i--) {
        v_ptr[0] /= Z;
        v_ptr[1] /= Z;
        v_ptr += 2;
    }

    if(n % 2 != 0)
        *v_ptr /= Z;

    return std::log(Z) + maxValue;
}

template<class KREAL>
void KtuMath<KREAL>::scale(KREAL x[], unsigned n, KREAL alpha)
{
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        x[i    ] *= alpha;
        x[i + 1] *= alpha;
        x[i + 2] *= alpha;
        x[i + 3] *= alpha;
    }
    for (; i < n; i++)
        x[i] *= alpha;
}

template<class KREAL>
void KtuMath<KREAL>::shift(KREAL x[], unsigned n, KREAL dc)
{
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        x[i    ] += dc;
        x[i + 1] += dc;
        x[i + 2] += dc;
        x[i + 3] += dc;
    }
    for (; i < n; i++)
        x[i] += dc;
}

template<class KREAL>
KREAL KtuMath<KREAL>::sum(const KREAL x[], unsigned n)
{
    KREAL tag = 0;
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        tag += x[i];
        tag += x[i+1];
        tag += x[i+2];
        tag += x[i+3];
    }

    for (; i < n; i++)
        tag += x[i];

    return tag;
}

template<class KREAL>
KREAL KtuMath<KREAL>::sumAbs(const KREAL x[], unsigned n)
{
    KREAL tag = 0;
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        tag += std::abs(x[i]);
        tag += std::abs(x[i + 1]);
        tag += std::abs(x[i + 2]);
        tag += std::abs(x[i + 3]);
    }

    for (; i < n; i++)
        tag += std::abs(x[i]);

    return tag;
}

template<class KREAL>
KREAL KtuMath<KREAL>::sum2(const KREAL x[], unsigned n)
{
    KREAL tag = 0;
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        tag += x[i] * x[i];
        tag += x[i + 1] * x[i + 1];
        tag += x[i + 2] * x[i + 2];
        tag += x[i + 3] * x[i + 3];
    }

    for (; i < n; i++)
        tag += x[i] * x[i];

    return tag;
}

template<class KREAL>
KREAL KtuMath<KREAL>::sumPower(const KREAL x[], unsigned n, KREAL power)
{
    KREAL tag(1.0);
    for (unsigned i = 0; i < n; i++)
        tag += std::pow(std::abs(x[i]), power);

    return tag;
}

template<class KREAL>
KREAL KtuMath<KREAL>::norm(const KREAL x[], unsigned n, KREAL power)
{
    if (power < 0) return nan;
    if (power == 2) {
        double sum2 = sum2(x, n);
        return std::sqrt(sum2);
    }
    else if (power == 1) {
        return sumAbs(x, n);
    }
    else {
        KREAL sumPower = sumPower(x, power);
        return std::pow(sumPower, 1 / power);
    }
}

template<class KREAL>
void KtuMath<KREAL>::normalize(const KREAL x[], unsigned n, KREAL power, KREAL newNorm)
{
    assert(newNorm > 0);
    KREAL oldnorm = norm(x, n, power);
    if (oldnorm > 0.0)
        scale(x, n, newNorm / oldnorm);
}

template<class KREAL>
KREAL KtuMath<KREAL>::product(const KREAL x[], unsigned n)
{
    KREAL tag(1);
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        tag *= x[i];
        tag *= x[i + 1];
        tag *= x[i + 2];
        tag *= x[i + 3];
    }

    for (; i < n; i++)
        tag *= x[i];

    return tag;
}

template<class KREAL>
KREAL KtuMath<KREAL>::var(const KREAL x[], unsigned n, KREAL mean)
{
    if(n > 1) {
        KREAL sum2 = 0;
        for (unsigned i = 0; i < n; i++)
            sum2 += (x[i] - mean) * (x[i] - mean);

        return sum2 / n;
    }

    return nan;
}

template<class KREAL>
KREAL KtuMath<KREAL>::dot(const KREAL x[], const KREAL y[], unsigned n)
{
    KREAL d = 0;

    const unsigned nDiv4 = n / 4;
    const unsigned nMod4 = n % 4;

    for(unsigned i = nDiv4; i != 0; i--) {
        d += x[0] * y[0] + x[1] * y[1] + x[2] * y[2] + x[3] * y[3];
        x += 4; y += 4;
    }

    switch(nMod4) {
    case 3:
        d += x[2] * y[2];
    case 2:
        d += x[1] * y[1];
    case 1:
        d += x[0] * y[0];
    }

    return d;
}

template<class KREAL>
void KtuMath<KREAL>::add(const KREAL x[], const KREAL y[], KREAL z[], unsigned n)
{
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        z[i] = x[i] + y[i];
        z[i + 1] = x[i + 1] + y[i + 1];
        z[i + 2] = x[i + 2] + y[i + 2];
        z[i + 3] = x[i + 3] + y[i + 3];
    }
    for (; i < n; i++)
        z[i] = x[i] + y[i];
}

template<class KREAL>
void KtuMath<KREAL>::sub(const KREAL x[], const KREAL y[], KREAL z[], unsigned n)
{
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        z[i] = x[i] - y[i];
        z[i + 1] = x[i + 1] - y[i + 1];
        z[i + 2] = x[i + 2] - y[i + 2];
        z[i + 3] = x[i + 3] - y[i + 3];
    }
    for (; i < n; i++)
        z[i] = x[i] - y[i];
}

template<class KREAL>
void KtuMath<KREAL>::subMean(KREAL x[], unsigned n)
{
    shift(x, n, -mean(x, n));
}

template<class KREAL>
void KtuMath<KREAL>::mul(const KREAL x[], const KREAL y[], KREAL z[], unsigned n)
{
    unsigned i = 0;
    for(; i + 4 <= n; i += 4) {
        z[i] = x[i] * y[i];
        z[i+1] = x[i+1] * y[i+1];
        z[i+2] = x[i+2] * y[i+2];
        z[i+3] = x[i+3] * y[i+3];
    }
    for(; i < n; i++)
        z[i] = x[i] * y[i];
}

template<class KREAL>
KREAL KtuMath<KREAL>::mean(const KREAL x[], unsigned n)
{
    return sum(x, n) / n;
}

template<class KREAL>
KREAL KtuMath<KREAL>::nonZeroMean(const KREAL x[], unsigned n)
{
    KREAL tag = 0.0;
    unsigned nonZeros(0);
    for (unsigned i = 0; i < n; i++)
    {
        if (x[i] != 0.0)
        {
            tag += x[i];
            ++nonZeros;
        }
    }

    return tag / KREAL(nonZeros);
}

template<class KREAL>
void KtuMath<KREAL>::prob(KREAL x[]/*inout*/, unsigned n)
{
    KREAL sum = sum(x, n);
    if (sum == 0)
        assign(x, n, 0);
    else
        scale(x, n, 1.0 / sum);
}

template<class KREAL>
 KREAL KtuMath<KREAL>::min(const KREAL x[], unsigned n)
{
    auto iter = std::min_element(x, x + n);
    return *iter;
}

template<class KREAL>
KREAL KtuMath<KREAL>::max(const KREAL x[], unsigned n)
{
    auto iter = std::max_element(x, x + n);
    return *iter;
}

template<class KREAL>
std::pair<KREAL, KREAL> KtuMath<KREAL>::minmax(const KREAL x[], unsigned n)
{
    auto iter = std::minmax_element(x, x + n);
    return { *iter.first, *iter.second };
}

template<class KREAL>
unsigned KtuMath<KREAL>::argMin(const KREAL x[], unsigned n)
{
    return std::min_element(x, x + n) - x;
}

template<class KREAL>
unsigned KtuMath<KREAL>::argMax(const KREAL x[], unsigned n)
{
    return std::max_element(x, x + n) - x;
}

template<class KREAL>
unsigned KtuMath<KREAL>::argRand(const KREAL x[], unsigned n)
{
    KREAL cutoff = sum(x, n) * rand() / (KREAL)RAND_MAX;
    KREAL cumSum = 0;
    for(unsigned i = 0; i < n; i++) {
        cumSum += x[i];
        if(cumSum >= cutoff)
            return i;
    }

    return -1;
}

template<class KREAL>
std::pair<unsigned, unsigned> KtuMath<KREAL>::argMixMax(const KREAL x[], unsigned n)
{
    auto iter = std::minmax_element(x, x + n);
    return { iter.first - x, *iter.second - x };
}

template<class KREAL>
KREAL KtuMath<KREAL>::quantile(const KREAL a[], unsigned n, KREAL factor)
{
    KREAL place = factor * n + 0.5f;
    long left = std::floor(place);
    if(n < 1) return 0;
    if(n == 1) return a[0];
    if(left < 0) left = 0;
    if(left >= (long)n-1) left = n - 2;
    if(a[left + 1] == a[left]) return a[left];
    return a[left] + (place - left) * (a[left + 1] - a[left]);
}

template<class KREAL>
std::vector<KREAL> KtuMath<KREAL>::combAdd(const std::vector<KREAL>& x, const std::vector<KREAL>& y)
{
    std::vector<KREAL> result;

    if (x.size() == 0) {
        result.assign(y.begin(), y.end());
    }
    else if (y.size() == 0) {
        result.assign(x.begin(), x.end());
    }
    else {
        result.reserve(x.size() * y.size());
        for (size_t i = 0; i < x.size(); i++) {
            for (size_t j = 0; j < y.size(); j++) {
                result.push_back(x[i] + y[j]);
            }
        }
    }

    return result;
}

template<class KREAL>
std::vector<KREAL> KtuMath<KREAL>::combMul(const std::vector<KREAL>& x, const std::vector<KREAL>& y)
{
    std::vector<KREAL> result;

    if (x.size() == 0) {
        result.assign(y.begin(), y.end());
    }
    else if (y.size() == 0) {
        result.assign(x.begin(), x.end());
    }
    else {
        result.reserve(x.size() * y.size());
        for (size_t i = 0; i < x.size(); i++) {
            for (size_t j = 0; j < y.size(); j++) {
                result.push_back(x[i] * y[j]);
            }
        }
    }

    return result;
}

template<class KREAL>
unsigned KtuMath<KREAL>::getZeroCrossing(const KREAL x[], unsigned n)
{
    unsigned zcr(0);
    for (unsigned i = 1; i < n; i++)
        zcr += std::abs(sign(x[i - 1]) - sign(x[i]));

    return zcr >> 1;
}


template<class KREAL>
void KtuMath<KREAL>::applyFloor(KREAL x[], unsigned n, KREAL floor)
{
    for (unsigned i = 0; i < n; i++)
        x[i] = std::max(x[i], floor);
}

template<class KREAL>
void KtuMath<KREAL>::applyLog(KREAL x[], unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        x[i] = std::log(x[i]);
}

template<class KREAL>
void KtuMath<KREAL>::applyExp(KREAL x[], unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        x[i] = std::exp(x[i]);
}

template<class KREAL>
void KtuMath<KREAL>::assign(KREAL x[], unsigned n, KREAL val)
{
    std::fill(x, x + n, val);
}

template<class KREAL>
KREAL KtuMath<KREAL>::binomialCoeff(KREAL N, KREAL K)
{
    // 为了防止溢出，使用log将乘除法变换为加减法
    double c = 0;
    for (int i = int(K + 1); i <= N; i++) { c += std::log(i); }
    for (int j = 1; j <= (N - K); j++) { c -= std::log(j); }
    return std::exp(c);
}
