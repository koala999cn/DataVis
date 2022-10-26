#include "KuFileUtil.h"
#include <sys/stat.h>
#include <fstream>
#include "KuStrUtil.h"


bool KuFileUtil::time(const std::string& path, time_t& tCreate, time_t& tModify, time_t& tLastAccess)
{
	struct _stat64i32 buf;
	if (0 != ::_stat(path.c_str(), &buf))
		return false;

	tCreate = buf.st_ctime;
	tModify = buf.st_mtime;
	tLastAccess = buf.st_atime;

	return true;
}


__int64 KuFileUtil::size(const std::string& path)
{
	struct _stat64i32 buf;
	if (0 != ::_stat(path.c_str(), &buf))
		return -1;

	return buf.st_size;
}


int KuFileUtil::lines(const std::string& path, bool include_empty_line)
{
	std::ifstream ifs(path);
	if (!ifs) return -1;

	int line_num(0);
	std::string line;
	while (std::getline(ifs, line)) {
		if (line.empty() && !include_empty_line)
			continue;

		line_num++;
	}

	return line_num;
}


std::vector<std::string> KuFileUtil::readAllLines(const std::string& path, bool trim, bool omit_blank)
{
	std::vector<std::string> res;
	std::ifstream ifs(path);
	std::string line;
	while (std::getline(ifs, line)) {
		char* p = line.data();
		if (trim)
			p = KuStrUtil::trimInPlace(p);

		if (*p == '\0' && omit_blank)
			continue;

		res.push_back(p);
	}

	return res;
}


std::string KuFileUtil::readAsString(const std::string& path)
{
	std::string text;
	auto len = size(path);
	std::ifstream ifs(path);
	if (len > 0 && ifs) {
		text.resize(len);
		ifs.read(text.data(), len);
		assert(text[len] == '\0');
		auto pos = text.find_last_not_of('\0');
		text.erase(pos + 1);
	}
	
	return text;
}


bool KuFileUtil::writeString(const std::string& path, const std::string& text, bool bAppend)
{
	std::ofstream ofs(path, bAppend ? std::ios::app : std::ios::trunc);
	if (!ofs) return false;
	return ofs.write(text.data(), text.size()).good();
}
