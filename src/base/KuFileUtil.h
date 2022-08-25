#pragma once
#include "KuStrUtil.h"
#include <assert.h>


class KuFileUtil
{
public:
    static bool time(const std::string& path, time_t& createOrChangeTime, time_t& modifyTime, time_t& lastAccessTime);

    static __int64 size(const std::string& path); // return -1 when error occured

    // return -1 while error occured
    static int lines(const std::string& path, bool include_empty_line = true);

    static std::vector<std::string> readAllLines(const std::string& path, bool trim = true, bool skip_blank_line = true);
    static std::string readAsString(const std::string& path);

    static bool writeString(const std::string& path, const std::string& text, bool bAppend = false);

    template<typename T>
    static bool readData1d(const std::string& path, std::vector<T>& x);

    template<typename T>
    static bool readData2d(const std::string& path, std::vector<T>& x, std::vector<T>& y);

    template<typename T>
    static bool readData3d(const std::string& path, std::vector<T>& x, std::vector<T>& y, std::vector<T>& z);

    template<typename T>
    static bool writeData1d(const std::string& path, const T* x, unsigned size);

    template<typename T>
    static bool writeData1d(const std::string& path, std::vector<T>& x) {
        return writeData1d(path, x.data(), x.size());
	}

    template<typename T>
    static bool writeData2d(const std::string& path, const T* x, const T* y, size_t size);

    template<typename T>
    static bool writeData(const std::string& path, std::vector< std::vector<T> >& data, const char* delim = "\t");

private:
	KuFileUtil() { }
	~KuFileUtil() { }
};

template<typename T>
bool KuFileUtil::readData1d(const std::string& path, std::vector<T>& x)
{
	std::string text = readAsString(path);
    if (text.empty())
		return false;

    auto tokens = KuStrUtil::split(text, " \t\n\r\f\v,[](){}");

    x.reserve(tokens.size());
    for (unsigned n = 0; n < tokens.size(); n++)
	{
        assert(!tokens[n].empty());

		char heading = tokens[n][0];
		if (::isdigit(heading) || heading == '-' || heading == '.' 
			|| heading == '+' || heading == 'i' || heading == 'I') // +inf支持，TODO:词法合规性检测不严格
		{
            x.push_back(KuStrUtil::Str2Val<T>(tokens[n].c_str()));
		}
		else
		{
			// K_TRACE(_T("KuFileUtil::readData1D: ignore illegal data-string '%s'\n"), tokens[n].c_str());
		}
	}

	return true;
}

template<typename T>
bool KuFileUtil::readData2d(const std::string& path, std::vector<T>& x, std::vector<T>& y)
{
    std::vector<T> tmp;
    if (!readData1D(path, tmp))
		return false;

	if (tmp.size() % 2)
	{
		// K_TRACE(_T("KuFileUtil::readData2D: 读取的有效数据非偶数(共%d个数据)。\n"), tmp.size());
	}

	size_t n = 0;
	for (; n < tmp.size() - 1; n += 2)
	{
		x.push_back(tmp[n]);
		y.push_back(tmp[n + 1]);
	}

	if (n < tmp.size())
	{
		//K_ASSERT(n == tmp.size() - 1);
		x.push_back(tmp[n]);
		y.push_back(0);
	}

	return true;
}

template<typename T>
bool KuFileUtil::readData3d(const std::string& path, std::vector<T>& x, std::vector<T>& y, std::vector<T>& z)
{
    std::vector<T> tmp;
	if (!readData1D(tmp))
		return false;

	if (tmp.size() % 3)
	{
		//K_TRACE(_T("KuFileUtil::readData3D: 读取的有效数据非3的倍数(共%d个数据)。\n"), tmp.size());
	}


	size_t n = 0;
	for (; n < tmp.size() - 2; n += 3)
	{
		x.push_back(tmp[n]);
		y.push_back(tmp[n + 1]);
		y.push_back(tmp[n + 2]);
	}

	if (n < tmp.size())
	{
		x.push_back(tmp[n++]);
		y.push_back(n < tmp.size() ? tmp[n++] : 0);
		z.push_back(0);
		//K_ASSERT(n == tmp.size());
	}

	return true;
}

template<typename T>
bool KuFileUtil::writeData1d(const std::string& path, const T* data, unsigned size)
{
    //KtComPtr<KcFileWriter> w = new KcFileWriter(path);
	//if (!w->canWrite())
	//	return false;

	//for (size_t n = 0; n < data_number; n++)
	//	w->WriteLine(KuStrUtil::Val2Str(data[n]).c_str());

	return false;
}

template<typename T>
bool KuFileUtil::writeData2d(const std::string& path, const T* x, const T* y, size_t size)
{
    //KtComPtr<KcFileWriter> w = new KcFileWriter(path);
	//if (!w->canWrite())
	//	return false;

	//for (size_t n = 0; n < data_number; n++)
	//{
	//	std::string str = KuStrUtil::Val2Str(x[n]);
	//	str += "\t";
	//	str += KuStrUtil::Val2Str(y[n]);
	//	w->WriteLine(str.c_str());
	//}

	return false;
}

template<typename T>
bool KuFileUtil::writeData(const std::string& path, std::vector< std::vector<T> >& data, const char* delim)
{
    //KtComPtr<KcFileWriter> w = new KcFileWriter(path);
	//if (!w->canWrite())
	//	return false;

	//for (size_t row = 0; row < data.size(); row++)
	//{
	//	std::string line;
	//	for (size_t col = 0; col < data[row].size(); col++)
	//	{
	//		line += KuStrUtil::Val2Str(data[row][col]);
	//		line += delim;
	//	}
	//	w->WriteLine(line.c_str());
	//}

	return false;
}
