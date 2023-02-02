#pragma once
#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>
#include <assert.h>


class KuMath
{
public:

    /// 一些数学常量
    static constexpr auto pi = 3.1415926535897932384626433832795028841971693993751L;
    static constexpr auto e = 2.7182818284590452353602874713526624977572L;
    static constexpr auto sqrt2 = 1.4142135623730950488L;
    static constexpr auto sqrt3 = 1.732050807568877L;
    static constexpr auto golden_section = 0.6180339887498948482045868343656381177203L; // sqrt(5)/2-0.5

    template<typename KREAL>
    static constexpr KREAL nan() { return std::numeric_limits<KREAL>::quiet_NaN(); }

    template<typename KREAL>
    static constexpr KREAL inf() { return std::numeric_limits<KREAL>::infinity(); }

    template<typename KREAL>
    static constexpr KREAL eps() { return std::numeric_limits<KREAL>::epsilon(); }


    /*************** SCALAR ALGORITHM *****************/

    template<typename KREAL>
    static bool isUndefined(KREAL x) { return std::isnan(x) || std::isinf(x); }

    template<typename KREAL>
    static bool isDefined(KREAL x) { return std::isfinite(x); }

    template<typename KREAL>
    static bool containUndefined(const KREAL x[], unsigned n);

    // return -1 for x < 0, 1 for x > 0, 0 for x == 0
    template<typename KREAL>
    static int sign(KREAL x) { return (x > 0) ? 1 : ((x < 0) ? -1 : 0); }

    // return the integer part of the input number
    template<typename KREAL>
    static int trunc(KREAL x) { 
        return (x >= 0) ? static_cast<int>(x) : -static_cast<int>(-x); 
    }

    // return the fractional part of the input number
    template<typename KREAL>
    static KREAL fract(KREAL x) { return x - std::floor<KREAL>(x); }

    // 消除非正规化数值影响.若val为极小值，则置为0.
    template<typename KREAL>
    static void killDenormal(KREAL& x); 

    // 近似等于比较，一个绝对值版本，一个相对值版本
    template<typename KREAL>
    static bool almostEqual(KREAL x1, KREAL x2, KREAL tol = 1e-7);
    template<typename KREAL>
    static bool almostEqualRel(KREAL x1, KREAL x2, KREAL rel_tol = 0.001);

    template<typename KREAL>
    static KREAL clampFloor(KREAL x, KREAL low) { return std::max(x, low); }

    template<typename KREAL>
    static KREAL clampCeil(KREAL x, KREAL high) { return std::min(x, high); }

    // 确保x在[low, high]之间
    template<typename KREAL>
    static KREAL clamp(KREAL x, KREAL low, KREAL high) { 
        return std::max(std::min(x, high), low); 
    } 

    template<typename KREAL>
    static KREAL absMax(KREAL x, KREAL y) {
        return std::max(std::abs(x), std::abs(y));
    }

    template<typename KREAL>
    static KREAL absMin(KREAL x, KREAL y) {
        return std::min(std::abs(x), std::abs(y));
    }

    // 把CLOSED参数放在前面，多数情况下KREAL可以推算出来
    template<bool CLOSED, typename KREAL>
    static bool inRange(KREAL x, KREAL low, KREAL high) {
        if constexpr (CLOSED)
            return x >= low && x <= high;
        else
            return x > low && x < high;
    }

    // 输入新值x，更新最小low和最大值high
    template<typename KREAL>
    static void updateRange(KREAL& low, KREAL& high, KREAL x) {
        if (x < low) low = x;
        if (x > high) high = x;
    }

    // (low0, high0) = (low0, high0) 并 (low1, high1)
    template<typename KREAL>
    static void uniteRange(KREAL& low0, KREAL& high0, KREAL low1, KREAL high1) {
        if (low1 < low0) low0 = low1;
        if (high1 > high0) high0 = high1;
    }

    // (low0, high0) = (low0, high0) 相交 (low1, high1)
    template<typename KREAL>
    static void intersectRange(KREAL& low0, KREAL& high0, KREAL low1, KREAL high1) {
        if (low1 > low0) low0 = low1;
        if (high1 < high0) high0 = high1;
        if (low0 > high0) low0 = high0;
    }

    template<typename KREAL>
    static constexpr KREAL deg2Rad(KREAL d) { return d * (pi / 180); }

    template<typename KREAL>
    static constexpr KREAL rad2Deg(KREAL r) { return r * (180 / pi); }

    template<typename KREAL>
    static KREAL mid(KREAL left, KREAL right) { return (left + right) / 2; }

    // 返回left和right在log域的中值
    template<typename KREAL>
    static KREAL logMid(KREAL left, KREAL right) { 
        assert(left > 0 && right > 0);
        return std::exp(mid(std::log(left), std::log(right))); 
    }


    // 基于base的对数值 --> 自然对数值
    // @ll: 待转换的基于base的对数值
    // @base: =0表示ll为线性域数值
    template<typename KREAL>
    static KREAL logFromBase(KREAL ll, KREAL base) {
        return base == 0 ? std::log(ll) : ll * std::log(base);
    }

    // 自然对数值 --> 基于base的对数值
    // @ll: 待转换的自然对数值
    // @base: =0表示将ll转换为线性域数值
    template<typename KREAL>
    static KREAL logToBase(KREAL ll, KREAL base) {
        return base == 0 ? std::exp(ll) : ll / std::log(base);
    }


    // 返回最大的整数x，以使得pow(10, x) <= val
    // 不等式两边取log10，有x <= log10(val)
    // @val: 必须大于0
    template<typename KREAL>
    static KREAL floorLog10(KREAL val) {
        return std::floor(std::log10(val));
    }

    // x = log(a), y = log(b)
    // return log(a+b)
    template<typename KREAL>
    static KREAL addLog(KREAL x, KREAL y);

    // x = log(a), y = log(b)
    // return log(a-b)
    template<typename KREAL>
    static KREAL subLog(KREAL x, KREAL y);

    // sinc(x) = sin(pi*x) / (pi*x)
    template<typename KREAL>
    static KREAL sinc(KREAL x);

    template<typename KREAL>
    static KREAL asinh(KREAL x) {
        return std::log(x + std::sqrt(x * x + 1));
    }

    template<typename KREAL>
    static KREAL acosh(KREAL x) {
        return std::log(x + std::sqrt(x * x - 1));
    }

    // logit function
    // Inverse of the sigmoidal logistic function
    // Also known as log-odds score or ratio
    template<typename KREAL>
    static KREAL logit(KREAL x) { return std::log(x / (1 - x)); }
    template<typename KREAL>
    static KREAL logistic(KREAL x) { return 1 / (1 + std::exp(-x)); }
    template<typename KREAL>
    static KREAL logistic(const KREAL theta[], const KREAL x[], unsigned n) {
        return logistic(-dot(theta, x, n));
    }

    // Calculate the binomial coefficient
    // It is the coefficient of the x^k term in the polynomial expansion of the binomial power (1+x)^n
    // it is fiven by the binomial formula:
    // B(n, k) = factorial(n) / [factorial(k) * factorial(n-k)]
    template<typename KREAL>
    static KREAL binomialCoeff(KREAL n, KREAL k);

    // Returns stirlings approximation of x!
    // x! ~= sqrt(2*pi*x)*(x/e)^x
    template<typename KREAL>
    static KREAL factorial(KREAL x) { // 实数版本的阶乘，使用stirlings近似。
        return std::sqrt(2 * pi * x) * std::pow(x / e, x);
    }

    // 一个基于stirlings，但精度更高的近似
    // http://www.rskey.org/CMS/index.php/the-library/11
    template<typename KREAL>
    static KREAL factorialX(KREAL x) {
        return factorial(x) * std::exp(1/(12*x+2/(5*x+53/(42*x))));
    }

    // 线性插值
    template<typename KREAL>
    static KREAL lerp(KREAL xmin, KREAL xmax, double x/*[0, 1]*/) {
        return xmin + (xmax - xmin) * x;
    } 

    // 重映射：将x从[x0, x1]区间映射到[y0, y1]区间
    template<typename T1, typename T2, bool CLAMP = false>
    static T2 remap(T1 x, T1 x0, T1 x1, T2 y0, T2 y1) {
        if (x0 == x1) 
            return x >= x1 ? y1 : y0;

        auto y = y0 + (y1 - y0) * ((x - x0) / (x1 - x0));
        if constexpr (CLAMP)
            y = clamp<T2>(y, y0, y1);
        return y;
    }

    template<typename KREAL, bool CLAMP = false>
    static KREAL remap(KREAL x, KREAL x0, KREAL x1, KREAL y0, KREAL y1) {
        return remap<KREAL, KREAL, CLAMP>(x, x0, x1, y0, y1);
    }

    // 规范化重映射：remap(x, x0, x1, 0, 1)
    template<typename T1, typename T2, bool CLAMP = false>
    static T2 remap(T1 x, T1 x0, T1 x1) {
        return remap<T1, T2, CLAMP>(x, x0, x1, 0, 1);
    }

    template<typename KREAL, bool CLAMP = false>
    static KREAL remap(KREAL x, KREAL x0, KREAL x1) {
        return remap<KREAL, KREAL, CLAMP>(x, x0, x1, 0, 1);
    }

    // 反转：相当于remap(x, x0, x1, x1, x0)
    template<typename KREAL>
    static KREAL invert(KREAL x, KREAL x0, KREAL x1) {
        return x0 + x1 - x;
    }

    /*************** VECTOR ALGORITHM *****************/
    template<typename KREAL>
    static void assign(KREAL x[], unsigned n, KREAL val);
    template<typename KREAL>
    static void zeros(KREAL x[], unsigned n) { assign<KREAL>(x, n, 0); }
    template<typename KREAL>
    static void ones(KREAL x[], unsigned n) { assign<KREAL>(x, n, 1); }

    template<typename KREAL>
    static KREAL quantile(const KREAL x[], unsigned n, KREAL factor);

    template<typename KREAL>
    static KREAL sum(const KREAL x[], unsigned n); // 各元素之和
    template<typename KREAL>
    static KREAL sumAbs(const KREAL x[], unsigned n); // 各元素绝对值之和
    template<typename KREAL>
    static KREAL sum2(const KREAL x[], unsigned n); // 各元素平方之和
    template<typename KREAL>
    static KREAL sumPower(const KREAL x[], unsigned n, KREAL power); // 各元素绝对值的power次方之和
    template<typename KREAL>
    static KREAL norm(const KREAL x[], unsigned n, KREAL power); // power阶范式


    // 各元素之积
    template<typename KREAL>
    static KREAL product(const KREAL x[], unsigned n); // 各元素之积

    // 平均值
    template<typename KREAL>
    static KREAL mean(const KREAL x[], unsigned n) { return sum(x, n) / n; }
    
    // 方差：Sum((x[i]-mean)^2) / n
    // @UNBIASED: true代表无偏估计，结果为Sum((x[i]-mean)^2) / (n-1)
    template<typename KREAL, bool UNBIASED = false>
    static KREAL var(const KREAL x[], unsigned n, KREAL mean);

    template<typename KREAL, bool UNBIASED = false>
    static KREAL var(const KREAL x[], unsigned n) { 
        return var<KREAL, UNBIASED>(x, n, mean(x, n));
    }

    // 均方差：sqrt(var)
    template<typename KREAL, bool UNBIASED = false>
    static KREAL stdDev(const KREAL x[], unsigned n) { 
        return std::sqrt(var<KREAL, UNBIASED>(x, n));
    }

    // Evaluate mean of a vector from elements that are not zeros
    template<typename KREAL>
    static KREAL nonZeroMean(const KREAL x[], unsigned n);

    template<typename KREAL>
    static void scale(KREAL x[], unsigned n, KREAL alpha); // x[i] *= alpha
    template<typename KREAL>
    static void scale(const KREAL x[], KREAL y[], unsigned n, KREAL alpha); // y[i] = x[i] * alpha
    template<typename KREAL>
    static void shift(KREAL x[], unsigned n, KREAL scalar); // x[i] += scalar
    template<typename KREAL>
    static void shift(const KREAL x[], KREAL y[], unsigned n, KREAL alpha); // y[i] = x[i] + alpha
    template<typename KREAL>
    static void subMean(KREAL x[], unsigned n); // x[i] -= mean
    template<typename KREAL>
    static void recip(KREAL x[], unsigned n); // 倒数: x[i] = 1 / x[i]
    template<typename KREAL>
    static void recip(const KREAL x[], KREAL r[], unsigned n); // 倒数: r[i] = 1 / x[i]

    template<typename KREAL>
    static void add(const KREAL x[], const KREAL y[], KREAL r[], unsigned n); // r[i] = x[i] + y[i]
    template<typename KREAL>
    static void sub(const KREAL x[], const KREAL y[], KREAL r[], unsigned n); // r[i] = x[i] - y[i]
    template<typename KREAL>
    static void mul(const KREAL x[], const KREAL y[], KREAL r[], unsigned n); // r[i] = x[i] * y[i]
    template<typename KREAL>
    static void div(const KREAL x[], const KREAL y[], KREAL r[], unsigned n); // r[i] = x[i] / y[i]

    template<typename KREAL>
    static KREAL dot(const KREAL x[], const KREAL y[], unsigned n); // Sum(x[i]*y[i])

    // 加权加法：r[i] = phase1 * x[i] + phase2 * y[i]
    template<typename KREAL>
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
    template<typename KREAL>
    static std::vector<KREAL> combAdd(const std::vector<KREAL>& x, const std::vector<KREAL>& y);

    /*! \fn std::vector<KREAL> combMul(std::vector< REAL >& x, std::vector< REAL >& y)
    \brief Multiply the x and y vector combinatorially in result
    \param x vector of (ints or doubles)
    \param y vector of (ints or doubles)
    \return the result of the two vectors muled together
    */
    template<typename KREAL>
    static std::vector<KREAL> combMul(const std::vector<KREAL>& x, const std::vector<KREAL>& y);

    // 过零数和过零率
    template<typename KREAL>
    static unsigned zeroCrossing(const KREAL x[], unsigned n);
    template<typename KREAL>
    static KREAL zcr(const KREAL x[], unsigned n) { return  KREAL(zeroCrossing(x, n)) / (n - 1); }

    template<typename KREAL>
    static void applyFloor(KREAL x[], unsigned n, KREAL floor); // X = max(X, floor)
    template<typename KREAL>
    static void applyExp(KREAL x[], unsigned n); // X = exp(X)
    template<typename KREAL>
    static void applyLog(KREAL x[], unsigned n); // X = log(X)


    // computes the gini impurity of a possibly unnormalized distribution
    // return 1.0 - Sum(x^2) / Sum(x)^2
    template<typename KREAL>
    static KREAL gini(const KREAL x[], unsigned n); // 支持KREAL=int的情况

    // exponentiates and normalizes a vector
    template<typename KREAL>
    static KREAL expAndNorm(KREAL x[], unsigned n);

    // 对x进行缩放，确保absMax(x) = val
    template<typename KREAL>
    static void scaleTo(KREAL x[], unsigned n, KREAL val);

    // x[i] *= i/(n-1)
    template<typename KREAL>
    static void fadeIn(KREAL x[], unsigned n);

    // x[i] *= (n-i-1)/(n-1)
    template<typename KREAL>
    static void fadeOut(KREAL x[], unsigned n);

    ////////////////////////////////////////////////////////////////////////

    // computes the entropy of a possibly unnormalized distribution
    // 数学定义为：E(X) = -Sum{ P(xi) * log(P(xi)) }
    // 物理解释：根据Shannon信息论，给定一个字符集的概率分布，可以设计一种编码，
    // 使得表示该字符集组成的字符串平均需要的比特数最少。假设这个字符集是X，
    // 对于其中某个字符xi，其出现概率为P(xi)，那么其最优编码（哈夫曼编码）
    // 平均需要的比特数等于这个字符集的熵。
    // 此处实现为了处理非归一化情况，实际求解E(px/sum(px))
    template<typename KREAL>
    static KREAL entropy(const KREAL x[], unsigned n); // 支持KREAL=int的情况

    // Shannon'tag Relative entropy (Kullback-Liebler Convergence)
    // Normalized for A->B and B->A
    // KL散度用于计算两个随机变量的差异程度，又称为信息增益、相对熵。
    // A关于B的KL散度数学定义为：
    //   KL(A || B) = E(A) - Sum{ PA(xi) * log(PB(xi)) }
    // 由上式可以看出，KL散度是不对称的，即A关于B的KL散度不等于B关于A的KL散度
    // 此处实现没有严格使用KL散度的数学定义，而是返回0.5(KL(A||B) + KL(B||A))
    template<typename KREAL>
    static KREAL relEntropy(const KREAL x[], const KREAL y[], unsigned n);

    // 交叉熵的意义是：使用随机变量B的最优编码方式对随机变量A编码所需要的字符数。
    //   具体来说就是使用哈夫曼编码却根据B的概率分布对A进行编码，所需要的编码数。
    // 交叉熵 = KL散度 + 熵
    template<typename KREAL>
    static KREAL crossEntropy(const KREAL x[], const KREAL y[], unsigned n);


    /*************** SEARCH ALGORITHM *****************/

    template<typename KREAL>
    static KREAL min(KREAL x, KREAL y, KREAL z) {
        return std::min(std::min(x, y), z);
    }
    template<typename KREAL>
    static KREAL max(KREAL x, KREAL y, KREAL z) {
        return std::max(std::max(x, y), z);
    }

    template<typename KREAL>
    static KREAL min(KREAL x, KREAL y, KREAL z, KREAL w) {
        return std::min(min(x, y, z), w);
    }
    template<typename KREAL>
    static KREAL max(KREAL x, KREAL y, KREAL z, KREAL w) {
        return std::max(max(x, y, z), w);
    }

    template<typename KREAL>
    static KREAL min(const KREAL x[], unsigned n);
    template<typename KREAL>
    static KREAL max(const KREAL x[], unsigned n);
    template<typename KREAL>
    static std::pair<KREAL, KREAL> minmax(const KREAL x[], unsigned n);

    template<typename KREAL>
    static unsigned argMin(const KREAL x[], unsigned n); // 返回最小值的索引[0, dim)
    template<typename KREAL>
    static unsigned argMax(const KREAL x[], unsigned n); // 返回最大值的索引[0, dim)
    template<typename KREAL>
    static unsigned argRand(const KREAL x[], unsigned n); // 返回累加和大于随机值的索引[-1, dim)
    template<typename KREAL>
    static std::pair<unsigned, unsigned> argMixMax(const KREAL x[], unsigned n);

    // 返回x中与val最接近的值
    // 要求x已升序排序
    template<typename KREAL>
    static KREAL pickNearest(KREAL val, const KREAL x[], unsigned n);

    // x[i] = op(x[i])
    template<typename KREAL, typename UNARY_OP>
    static void forEach(KREAL x[], unsigned n, UNARY_OP op);

    // r[i] = op(x[i])
    template<typename KREAL, typename UNARY_OP>
    static void forEach(const KREAL x[], KREAL r[], unsigned n, UNARY_OP op);

    // r[i] = op(x[i], y[i])
    template<typename KREAL, typename BINARY_OP>
    static void forEach(const KREAL x[], const KREAL y[], KREAL r[], unsigned n, BINARY_OP op);

    /*************** others *****************/

    // 将区间<left, right>等间隔分为olen个点，结果存放到out
    // @x0ref: 第一个分割点相对(dx)起始位置，x0 = x0ref * dx
    // @RIGHT_CLOSED: 若true, 则最后一个分割点<=right, 否则<right
    // @olen: 等分点的数量
    template<typename KREAL, bool RIGHT_CLOSED = true>
    static void linspace(KREAL left, KREAL right, KREAL x0ref, KREAL* out, unsigned olen);

private:
    KuMath() = delete;
    ~KuMath() = delete;
};


// 参考Laurent de Soras. Denormal numbers in floating point signal processing applications.
template<typename KREAL>
void KuMath::killDenormal(KREAL& x)
{
    static constexpr KREAL antiDenormal = 1e-18;
    x += antiDenormal;
    x -= antiDenormal;
}


template<typename KREAL>
bool KuMath::almostEqual(KREAL x1, KREAL x2, KREAL tol)
{
    return std::abs(x1 - x2) <= tol;
}


template<typename KREAL>
bool KuMath::almostEqualRel(KREAL x1, KREAL x2, KREAL rel_tol)
{
    // a==b handles infinities.
    if(x1 == x2) 
        return true;

    if (almostEqual(x1, 0) || almostEqual(x2, 0)) 
        return almostEqual(x1, x2); // TODO: 

    double diff = std::abs(x1 - x2);
    if(isUndefined(diff))
        return false;

    return diff <= rel_tol*(std::abs(x1) + std::abs(x2));
}


template<typename KREAL>
KREAL KuMath::addLog(KREAL x, KREAL y)
{
    if (x == -inf) {
        return y;
    }
    else if (y == -inf) {
        return x;
    }
    else if (x > y) {
        return x + log(1 + std::exp(y - x));
    }
    else {
        return y + log(1 + std::exp(x - y));
    }
}


template<typename KREAL>
KREAL KuMath::subLog(KREAL x, KREAL y)
{
    if (x < y) // log(-k)
        return nan;

    else if (x == y) // log(0)
        return -inf;

    else if (y == -inf) // log(x-0) = log(x) = x
        return x;

    else
        return x + log(1 - std::exp(y - x));
}


template<typename KREAL>
KREAL KuMath::sinc(KREAL x)
{
    KREAL _x = pi * x;

    // _x ~ 0 approximation
    // from : http://mathworld.wolfram.com/SincFunction.html
    // sinc(z) = \prod_{k=1}^{\infty} { cos(\pi z / 2^k) }
    if (abs(_x) < 0.01)
        return std::cos(_x / 2)*std::cos(_x / 4)*std::cos(_x / 8);

    return std::sin(_x) / _x;
}


template<typename KREAL>
bool KuMath::containUndefined(const KREAL x[], unsigned n)
{
    for(unsigned i = 0; i < n; i++) {
        if(isUndefined(*x++))
            return true;
    }

    return false;
}


template<typename KREAL>
KREAL KuMath::entropy(const KREAL x[], unsigned n)
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


template<typename KREAL>
KREAL KuMath::relEntropy(const KREAL x[], const KREAL y[], unsigned n)
{
    KREAL rel_entropy = 0;

    for (unsigned i = 0; i < n; i++)
        rel_entropy += (x[i] * std::log2(x[i] / y[i]) + y[i] * std::log2(y[i] / x[i])) / 2;

    return rel_entropy;
}


template<typename KREAL>
KREAL KuMath::crossEntropy(const KREAL x[], const KREAL y[], unsigned n)
{
    KREAL cross_entropy = 0;

    for (unsigned i = 0; i < n; i++)
        cross_entropy += x[i] * std::log2(y[i]);

    return -cross_entropy;
}


template<typename KREAL>
KREAL KuMath::gini(const KREAL x[], unsigned n)
{
    KREAL Z = 0;
    KREAL G = 0;

    for(unsigned i = 0; i < n; i++) {
        G += x[i] * x[i];
        Z += x[i];
    }

    return 1.0 - G / (Z * Z);
}


template<typename KREAL>
KREAL KuMath::expAndNorm(KREAL x[], unsigned n)
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


template<typename KREAL>
void KuMath::scale(KREAL x[], unsigned n, KREAL alpha)
{
    forEach(x, n, [alpha](KREAL x) { return x * alpha; });
}


template<typename KREAL>
void KuMath::scale(const KREAL x[], KREAL y[], unsigned n, KREAL alpha)
{
    forEach(x, y, n, [alpha](KREAL x) { return x * alpha; });
}


template<typename KREAL>
void KuMath::shift(KREAL x[], unsigned n, KREAL dc)
{
    forEach(x, n, [dc](KREAL x) { return x + dc; });
}


template<typename KREAL>
void KuMath::shift(const KREAL x[], KREAL y[], unsigned n, KREAL dc)
{
    forEach(x, y, n, [dc](KREAL x) { return x + dc; });
}


template<typename KREAL>
KREAL KuMath::sum(const KREAL x[], unsigned n)
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


template<typename KREAL>
KREAL KuMath::sumAbs(const KREAL x[], unsigned n)
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


template<typename KREAL>
KREAL KuMath::sum2(const KREAL x[], unsigned n)
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


template<typename KREAL>
KREAL KuMath::sumPower(const KREAL x[], unsigned n, KREAL power)
{
    KREAL tag(1.0);
    for (unsigned i = 0; i < n; i++)
        tag += std::pow(std::abs(x[i]), power);

    return tag;
}


template<typename KREAL>
KREAL KuMath::norm(const KREAL x[], unsigned n, KREAL power)
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


template<typename KREAL>
KREAL KuMath::product(const KREAL x[], unsigned n)
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


template<typename KREAL, bool UNBIASED>
KREAL KuMath::var(const KREAL x[], unsigned n, KREAL mean)
{
    if(n > 1) {
        KREAL sum2 = 0;
        for (unsigned i = 0; i < n; i++)
            sum2 += (x[i] - mean) * (x[i] - mean);

        return UNBIASED ? sum2 / (n - 1) : sum2 / n;
    }

    return nan;
}


template<typename KREAL>
KREAL KuMath::dot(const KREAL x[], const KREAL y[], unsigned n)
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


template<typename KREAL>
void KuMath::subMean(KREAL x[], unsigned n)
{
    shift(x, n, -mean(x, n));
}


template<typename KREAL>
void KuMath::recip(KREAL x[], unsigned n)
{
    forEach(x, n, [](KREAL x) { return 1 / x; });
}


template<typename KREAL>
void KuMath::recip(const KREAL x[], KREAL r[], unsigned n)
{
    forEach(x, r, n, [](KREAL x) { return 1 / x; });
}


template<typename KREAL>
void KuMath::add(const KREAL x[], const KREAL y[], KREAL r[], unsigned n)
{
    forEach(x, y, r, n, [](KREAL x, KREAL y) { return x + y; });
}


template<typename KREAL>
void KuMath::sub(const KREAL x[], const KREAL y[], KREAL r[], unsigned n)
{
    forEach(x, y, r, n, [](KREAL x, KREAL y) { return x - y; });
}


template<typename KREAL>
void KuMath::mul(const KREAL x[], const KREAL y[], KREAL r[], unsigned n)
{
    forEach(x, y, r, n, [](KREAL x, KREAL y) { return x * y; });
}


template<typename KREAL>
void KuMath::div(const KREAL x[], const KREAL y[], KREAL r[], unsigned n)
{
    forEach(x, y, r, n, [](KREAL x, KREAL y) { return x / y; });
}


template<typename KREAL>
KREAL KuMath::nonZeroMean(const KREAL x[], unsigned n)
{
    KREAL tag = 0.0;
    unsigned nonZeros(0);
    for (unsigned i = 0; i < n; i++)
    {
        if (x[i] != 0) {
            tag += x[i];
            ++nonZeros;
        }
    }

    return tag / KREAL(nonZeros);
}


template<typename KREAL>
 KREAL KuMath::min(const KREAL x[], unsigned n)
{
    auto iter = std::min_element(x, x + n);
    return *iter;
}


template<typename KREAL>
KREAL KuMath::max(const KREAL x[], unsigned n)
{
    auto iter = std::max_element(x, x + n);
    return *iter;
}


template<typename KREAL>
std::pair<KREAL, KREAL> KuMath::minmax(const KREAL x[], unsigned n)
{
    auto iter = std::minmax_element(x, x + n);
    return { *iter.first, *iter.second };
}


template<typename KREAL>
unsigned KuMath::argMin(const KREAL x[], unsigned n)
{
    return std::min_element(x, x + n) - x;
}


template<typename KREAL>
unsigned KuMath::argMax(const KREAL x[], unsigned n)
{
    return std::max_element(x, x + n) - x;
}


template<typename KREAL>
unsigned KuMath::argRand(const KREAL x[], unsigned n)
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


template<typename KREAL>
std::pair<unsigned, unsigned> KuMath::argMixMax(const KREAL x[], unsigned n)
{
    auto iter = std::minmax_element(x, x + n);
    return { iter.first - x, *iter.second - x };
}


template<typename KREAL>
KREAL KuMath::quantile(const KREAL a[], unsigned n, KREAL factor)
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


template<typename KREAL>
std::vector<KREAL> KuMath::combAdd(const std::vector<KREAL>& x, const std::vector<KREAL>& y)
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


template<typename KREAL>
std::vector<KREAL> KuMath::combMul(const std::vector<KREAL>& x, const std::vector<KREAL>& y)
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


template<typename KREAL>
unsigned KuMath::zeroCrossing(const KREAL x[], unsigned n)
{
    unsigned zcr(0);
    for (unsigned i = 1; i < n; i++)
        zcr += std::abs(sign(x[i - 1]) - sign(x[i]));

    return zcr >> 1;
}


template<typename KREAL>
void KuMath::applyFloor(KREAL x[], unsigned n, KREAL floor)
{
    for (unsigned i = 0; i < n; i++)
        x[i] = std::max(x[i], floor);
}


template<typename KREAL>
void KuMath::applyLog(KREAL x[], unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        x[i] = std::log(x[i]);
}


template<typename KREAL>
void KuMath::applyExp(KREAL x[], unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        x[i] = std::exp(x[i]);
}


template<typename KREAL>
void KuMath::assign(KREAL x[], unsigned n, KREAL val)
{
    std::fill_n(x, n, val);
}


template<typename KREAL>
void KuMath::scaleTo(KREAL x[], unsigned n, KREAL val)
{
    auto mm = minmax(x, n);
    auto factor = val / absMax(mm.first, mm.second);
    scale(x, n, factor);
}


template<typename KREAL>
KREAL KuMath::binomialCoeff(KREAL N, KREAL K)
{
    // 为了防止溢出，使用log将乘除法变换为加减法
    double c = 0;
    for (int i = int(K + 1); i <= N; i++) { c += std::log(i); }
    for (int j = 1; j <= (N - K); j++) { c -= std::log(j); }
    return std::exp(c);
}


template<typename KREAL, typename UNARY_OP>
void KuMath::forEach(KREAL x[], unsigned n, UNARY_OP op)
{
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        x[i] = op(x[i]);
        x[i + 1] = op(x[i + 1]);
        x[i + 2] = op(x[i + 2]);
        x[i + 3] = op(x[i + 3]);
    }
    for (; i < n; i++)
        x[i] = op(x[i]);
}


template<typename KREAL, typename UNARY_OP>
void KuMath::forEach(const KREAL x[], KREAL r[], unsigned n, UNARY_OP op)
{
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        r[i] = op(x[i]);
        r[i + 1] = op(x[i + 1]);
        r[i + 2] = op(x[i + 2]);
        r[i + 3] = op(x[i + 3]);
    }
    for (; i < n; i++)
        r[i] = op(x[i]);
}


template<typename KREAL, typename BINARY_OP>
void KuMath::forEach(const KREAL x[], const KREAL y[], KREAL r[], unsigned n, BINARY_OP op)
{
    unsigned i = 0;
    for (; i + 4 <= n; i += 4) {
        r[i] = op(x[i], y[i]);
        r[i + 1] = op(x[i + 1], y[i + 1]);
        r[i + 2] = op(x[i + 2], y[i + 2]);
        r[i + 3] = op(x[i + 3], y[i + 3]);
    }
    for (; i < n; i++)
        r[i] = op(x[i], y[i]);
}


template<typename KREAL, bool RIGHT_CLOSED>
void KuMath::linspace(KREAL left, KREAL right, KREAL x0ref, KREAL* out, unsigned olen)
{
    assert(x0ref >= 0 && x0ref < 1);

    if (olen == 0)
        return;
    else if (olen == 1) {
        *out = left + (right - left) * x0ref;
        return;
    }

    auto dx = right - left;
    if constexpr (RIGHT_CLOSED) {
        dx /= x0ref == 0 ? olen - 1 : olen;
    }
    else
        dx /= olen;

    for (unsigned i = 0; i < olen; i++)
        out[i] = left + (x0ref + i) * dx;
}


template<typename KREAL>
void KuMath::fadeIn(KREAL x[], unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        x[i] *= KREAL(i) / KREAL(n - 1);
}


template<typename KREAL>
void KuMath::fadeOut(KREAL x[], unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        x[i] *= KREAL(n - i - 1) / KREAL(n - 1);
}


template<typename KREAL>
KREAL KuMath::pickNearest(KREAL val, const KREAL x[], unsigned n)
{
    if (n < 2) return val;
   
    auto iter = std::lower_bound(x, x + n, val);

    if (iter == x + n)
        return *(iter - 1);
    else if (iter == x)
        return *iter;
    else
        return (val - *(iter - 1)) < (*iter - val) ? *(iter - 1) : *iter;
}
