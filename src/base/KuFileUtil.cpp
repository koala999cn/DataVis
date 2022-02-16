#include "KuFileUtil.h"
#include "base/KuFilePath.h"
#include "KcFileReader.h"
#include "KtComPtr.h"
#include <sys/stat.h>


bool KuFileUtil::getTime(const TCHAR* file, time_t& tCreate, time_t& tModify, time_t& tLastAccess)
{
	struct _stat64i32 buf;
	if (0 != ::_tstat(file, &buf))
		return false;

	tCreate = buf.st_ctime;
	tModify = buf.st_mtime;
	tLastAccess = buf.st_atime;

	return true;
}

__int64 KuFileUtil::getSize(const TCHAR* file)
{
	struct _stat64i32 buf;
	if (0 != ::_tstat(file, &buf))
		return -1;

	return buf.st_size;
}

int KuFileUtil::getLineCount(const TCHAR* file, bool include_empty_line)
{
	if (!KuFilePath::IsExist(file))
		return -1;

	KtComPtr<KiReader> r = new KcFileReader(file);
	int line_num(0);
	std::vector<char> line;
	while (r->getLine(line))
	{
		if (line.empty() && !include_empty_line)
			continue;

		line_num++;
	}

	return line_num;
}

bool KuFileUtil::readAllLines(const TCHAR* file, std::vector<std::string>& lines, bool trim, bool omit_blank)
{
	KtComPtr<KiReader> r = new KcFileReader(file);

	if (!r->canRead())
		return false;

	std::vector<char> line;
	while (r->getLine(line))
	{
		char* p = line.data();
		if (trim)
			p = KuStrUtil::TrimInPlace(p);

		if (*p == '\0' && omit_blank)
			continue;

		lines.push_back(p);
	}

	return true;
}

bool KuFileUtil::readAsString(const TCHAR* filePath, std::string& text)
{
	KtComPtr<KiReader> r = new KcFileReader(filePath);

	if (!r->canRead())
		return false;

	r->readString(text);

	return true;
}

bool KuFileUtil::writeString(const TCHAR* filePath, std::string& text, bool bAppend)
{
	KtComPtr<KiWriter> w = new KcFileWriter(filePath, bAppend);
	if (!w->canWrite())
		return false;

	w->writeString(text);

	return true;
}
