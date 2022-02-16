#include "KuPathUtil.h"
#include <io.h>
#include <stdlib.h>
#include <direct.h>
#include <sys/stat.h>
#include <assert.h>
#include "KuStrUtil.h"


namespace kPrivate
{
	const char k_illegalFileNameChars[] =
		"\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC\xD\xE\xF"
		"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
		"\"/:<>\\|?*";

	const char* k_dosDeviceNames[] = {
		"CON",  "PRN"   "AUX",  "NUL",  "COM1", "COM2", "COM3", "COM4",
		"COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3",
		"LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
	};
}


std::string KuPathUtil::normSeps(const std::string& path, char normSep)
{
	auto npath = path;
	if (normSep == '/') {
		KuStrUtil::replaceChar(npath, '\\', '/');
		while (KuStrUtil::replaceSubstr(npath, "//", "/"));
	}
	else {
		assert(normSep == '\\');
		KuStrUtil::replaceChar(npath, '/', '\\');
		while (KuStrUtil::replaceSubstr(npath, "\\\\", "\\"));
	}

	return npath;
}


std::string KuPathUtil::splitRoot(const std::string& path)
{
	if (path.empty())
		return "";

	// 处理盘符开头的路径
	if (::isalpha(path[0]) && path[1] == ':')
		return path.substr(0, isSeperator(path[2]) ? 3 : 2);

	// 处理\开头的路径
	if (isSeperator(path[0]) && !isSeperator(path[1]))
		return path.substr(0, 1);

	if (path[0] != '\\' || path[1] != '\\' || path[2] == '\\')
		return "";

	// 处理UNC路径，返回前两个部分作为root
	static const std::string uncPrefix1 = "\\\\.\\UNC\\", uncPrefix2 = "\\\\?\\UNC\\";
	size_t initPos = 2;
	if (KuStrUtil::beginWith(path, uncPrefix1, true) ||
		KuStrUtil::beginWith(path, uncPrefix2, true))
		initPos = uncPrefix1.length();

	auto index1 = path.find('\\', initPos); // 定位hostname
	if (index1 == path.npos)
		return path; // 只包含hostname的UNC，将全部字符当成根路径

	auto index2 = path.find('\\', index1 + 1); // 定位sharename
	if (index2 == index1 + 1)
		return ""; // 再次出现\\，不符号UNC规范

	if (index2 == path.npos)
		return path;

	return path.substr(0, index2 + 1);
}


// 算法参考https://docs.microsoft.com/en-us/dotnet/standard/io/file-path-formats#path-normalization
// TODO: 1. Handle legacy DOS device. 2. Apply the current directory.
std::string KuPathUtil::norm(const std::string& path, char normSep)
{
	// 第一步：将path分裂为root和relpath，并作分隔符规范化
	auto root = splitRoot(path);
	std::string relpath = path.substr(root.length());
	if (!KuStrUtil::beginWith(root, "\\\\")) // 不对UNC路径的根作分隔符规范化
		root = normSeps(root, normSep);
	relpath = normSeps(relpath, normSep);


	// 第二步：处理relpath中的"../"和"./"，但保留前缀"../"，它们要和当前路径相结合才能解析

	// 先删除"./"
	std::string dotpath = "." + normSep;
	KuStrUtil::replaceSubstr(relpath, normSep + dotpath, std::string(1, normSep));
	while (KuStrUtil::beginWith(relpath, dotpath)) relpath.erase(0, dotpath.length());

	// 再解析"../"
	std::string prefix; // 用来保存前缀"../"
	std::string ddotpath = ".." + normSep;
	auto i = relpath.find(ddotpath);
	while (i != std::string::npos) {
		if (i == 0) { // 前缀"../"，保存它
			prefix += ddotpath;
			relpath.erase(0, ddotpath.length());
		}
		else if(relpath[i-1] == normSep) { // resolve "../"

			auto j = relpath.substr(0, i - 1).find_last_of(normSep);
			if (j == std::string::npos) j = 0;

			relpath.erase(j, i - j + 2);
		}

		i = relpath.find(ddotpath);
	}


	// 第三步：裁剪字符
	// 1. 如果某段以单个句点结尾，则删除此句点。 
	//    单个或两个句点的段在之前的步骤中已规范化。 
	//    三个或更多句点的段未规范化，并且实际上是有效的文件/目录名。
	// 2. 如果路径的结尾不是分隔符，则删除所有尾随句点和空格(U + 0020)。 
	//    如果最后的段只是单个或两个句点，则按上述relpath规则处理。

	if (KuStrUtil::endWith(relpath, normSep + "."))
		relpath.pop_back(); // 将结尾的"/."替换为"/"

	if (KuStrUtil::endWith(relpath, normSep + "..")) {
		relpath.pop_back(); relpath.pop_back(); relpath.pop_back();
		if (relpath.empty())
			prefix += ddotpath;
		else {
			relpath = parentPath(relpath);
		}
	}

	if (isSeperator(relpath.back()))
		KuStrUtil::trimRightInPlace(&relpath[0], " .");

	if (isAbsPath(root) && !prefix.empty()) {
		// TODO: 触发错误："../"跨越根目录
	}

	return root + prefix + relpath; 
}


std::pair<std::string, std::string> KuPathUtil::normRoot(const std::string& root)
{
	std::string hostname, sharename;
	bool splitHost = false; // 标记是否需要从sharename中提取hostname

	if (KuStrUtil::beginWith(root, "\\\\.\\") || KuStrUtil::beginWith(root, "\\\\?\\")) {
		hostname = root[2];
		sharename = root.substr(4);
		if (KuStrUtil::beginWith(sharename, "UNC\\", true)) {
			sharename.erase(0, 4);
			splitHost = true;
		}
	}
	else if (KuStrUtil::beginWith(root, "\\\\")) {
		sharename = root.substr(2);
		splitHost = true;
	}
	else {
		sharename = root;
	}

	if (splitHost) {
		hostname = splitFirstPart(sharename, '\\'); 

		if (!sharename.empty() && ::isalpha(sharename[0]) && sharename[1] == '$')
			sharename[1] = ':';

		// TODO: 支持Volume GUID， 以及\\.\BootPartition\ 
	}

	KuStrUtil::toLower(hostname);
	if (hostname == "." || hostname == "?" || hostname == "127.0.0.1" || hostname == "localhost")
		hostname.clear();
		
	return { hostname, sharename };
}


bool KuPathUtil::isSameRoot(const std::string& root1, const std::string& root2)
{
	auto nroot1 = normRoot(root1);
	auto nroot2 = normRoot(root2);

	return ::_stricmp(nroot1.first.c_str(), nroot2.first.c_str()) == 0
		&& ::_stricmp(nroot1.second.c_str(), nroot2.second.c_str()) == 0;
}


bool KuPathUtil::hasEndingSeperator(const std::string& path)
{
	return isSeperator(path.back());
}


void KuPathUtil::trimEndingSeperator(std::string& path)
{
	while (hasEndingSeperator(path))
		path.pop_back();
}


void KuPathUtil::assureEndingSeperator(std::string& path, char sep)
{
	if (!hasEndingSeperator(path))
		path.push_back(sep);
}


std::string KuPathUtil::join(const std::string& s1, const std::string& s2)
{
	std::string path(s1);

	if (!s2.empty()) {
		if (!isSeperator(s2[0]))
			assureEndingSeperator(path);
		else
			trimEndingSeperator(path);
	}

	return path + s2;
}


std::string KuPathUtil::makePath(const std::string& dir, const std::string& base, const std::string& ext)
{
	auto path = combine(dir, base);

	if (!ext.empty()) {
		if (ext[0] != '.') path += '.';
		path += ext;
	}

	return path;
}


std::string KuPathUtil::getRelPath(const std::string& fullPath, const std::string& parentPath)
{
	auto rootFull = splitRoot(fullPath);
	auto rootParent = splitRoot(parentPath);
	if (!isSameRoot(rootFull, rootParent))
		return fullPath;

	auto relFull = fullPath.substr(rootFull.length());
	relFull = norm(relFull); KuStrUtil::toLower(relFull);
	auto relParent = parentPath.substr(rootParent.length());
	relParent = norm(relParent); KuStrUtil::toLower(relParent);

	while (!relParent.empty()) {
		auto partFull = splitFirstPart(relFull, nativeSep);
		auto partParent = splitFirstPart(relParent, nativeSep);
		if (partFull != partParent)
			return fullPath;
	}

	return relFull;
}


std::string KuPathUtil::splitFirstPart(std::string& path, char sep)
{
	std::string firstPart;
	std::string::size_type pos;

	if (sep != '\0') {
		pos = path.find_first_of(sep);
	}
	else {
		auto p = KuStrUtil::findFirstOneOf(path.c_str(), kSeps);
		pos = p ? p - path.c_str() : path.npos;
	}

	if (pos != path.npos) {
		firstPart = path.substr(0, pos);
		path.erase(0, pos + 1);
	}
	else {
		firstPart = path;
		path.clear();
	}

	return firstPart;
}


std::string KuPathUtil::splitLastPart(std::string& path, char sep)
{
	std::string lastPart;
	std::string::size_type pos;

	if (sep != '\0') {
		pos = path.find_last_of(sep);
	}
	else {
		auto p = KuStrUtil::findLastOneOf(path.c_str(), kSeps);
		pos = p ? p - path.c_str() : path.npos;
	}

	if (pos != path.npos) {
		lastPart = path.substr(pos + 1);
		path.erase(pos);
	}
	else {
		lastPart = path;
		path.clear();
	}

	return lastPart;
}


std::pair<std::string, std::string> KuPathUtil::splitPath(const std::string& path)
{
	std::string dir, fileName;
	dir = path;
	trimEndingSeperator(dir);
	fileName = splitLastPart(dir, '\0');
	return { dir, fileName };
}


std::pair<std::string, std::string> KuPathUtil::splitFileName(const std::string& fileName, bool fullExt)
{
	std::string baseName, ext;
	std::string::size_type pos = fullExt ? fileName.find_first_of('.') : fileName.find_last_of('.');
	if (pos == std::string::npos) {
		baseName = fileName;
	}
	else {
		ext = fileName.substr(pos + 1);
		baseName = fileName.substr(0, pos);
	}

	return { baseName, ext };
}


bool KuPathUtil::isLegalFileName(const std::string& fileName)
{
	return KuStrUtil::findFirstOneOf((char*)fileName.c_str(), kPrivate::k_illegalFileNameChars) == nullptr;
}


bool KuPathUtil::isDosDeviceName(const std::string& fileName)
{
	auto N = sizeof(kPrivate::k_dosDeviceNames) / sizeof(char*);
	for (auto i = 0u; i < N; i++)
		if (KuStrUtil::beginWith(fileName, kPrivate::k_dosDeviceNames[i], true))
			return true; // 只要开头是deviceName，就认定该文件名是deviceName

	return false;
}


std::string KuPathUtil::parentPath(const std::string& path)
{
	auto root = splitRoot(path);
	if (!isAbsRoot(root))
	    return parentPath(toAbs(path));

	// 处理根路径为绝对路径的情况
	auto relpath = norm(path.substr(root.length()), nativeSep);
	trimEndingSeperator(relpath);

	// path是根路径或者相对路径"../"跨越根路径，返回空
	if (relpath.empty() || KuStrUtil::beginWith(relpath, ".." + nativeSep))
		return "";
		
	auto pos = relpath.find_last_of(nativeSep);
    return pos == std::string::npos ? root : relpath.substr(0, pos + 1);
}


std::string KuPathUtil::toAbs(const std::string& path)
{
	auto buf = ::_fullpath(0, path.c_str(), 0); // TODO: 非ms环境下，使用realpath
	std::string fpath(buf, buf + KuStrUtil::length(buf));
	::free(buf);
	return fpath;
}


std::string KuPathUtil::getCwd()
{
	char* buf = ::_getcwd(nullptr, 0);
	std::string cwd(buf, buf + KuStrUtil::length(buf));
	::free(buf);

	return cwd;
}


bool KuPathUtil::setCwd(const std::string& path)
{
	return 0 == ::_chdir(path.c_str());
}


bool KuPathUtil::exist(const std::string& path)
{
	return -1 != ::_access(path.c_str(), 0);
}


bool KuPathUtil::isDir(const std::string& path)
{
	struct _stat st;
	return 0 == ::_stat(path.c_str(), &st) && S_IFDIR & st.st_mode;
}


bool KuPathUtil::createDir(const std::string& path)
{
	if(!exist(path))
	{
		std::string parent = parentPath(path);
		if(!createDir(parent)) return false; // 递归创建父目录

		return 0 == ::_mkdir(path.c_str());
	}

	return true;
}


#ifdef _WIN32
#include <shlobj.h>
#include <tchar.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "shell32.lib")
		#pragma comment(lib, "user32.lib") // for GetDesktopWindow
    #endif
#endif
// 弹出目录选择对话框，返回用户选择的目录，返回空字符如果用户取消了操作
std::string KuPathUtil::browse(const std::string& szPathToShow)
{
#ifdef _WIN32
	BROWSEINFOA bi;
	char szDisplayName[MAX_PATH];
	::memset(&bi, 0x00, sizeof(bi));
	bi.hwndOwner = ::GetDesktopWindow();
	bi.pszDisplayName = szDisplayName;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle = "Please choose the path"; // _T("请选择路径");
	LPITEMIDLIST pidl = ::SHBrowseForFolderA(&bi);
	if(pidl)
	{
		std::string strOldCurPath = getCwd(); // save the old current-directory
		setCwd(szPathToShow); // Don't mind the result, let it be!
		::SHGetPathFromIDListA(pidl, szDisplayName); // show the dialog
		setCwd(strOldCurPath); // restore the old current-directory

		// free the pidl
		IMalloc* spSHMalloc;
		auto r = ::SHGetMalloc(&spSHMalloc);
		if(r == S_OK)
			spSHMalloc->Free(pidl);
	}
	else
		szDisplayName[0] = 0;

	return szDisplayName;
#else
	#error KuPathUtil::browse() NOT implemented!
#endif
}


bool KuPathUtil::copyFiles(const std::string& fromPath, const std::string& toPath, bool includeSubDirs)
{
	if (!isDir(fromPath) || !createDir(toPath))
		return false;

	auto files = getFiles(fromPath, includeSubDirs, true); // TODO: 待优化，当文件量很大时容易出现问题

	for (auto& f : files) {
		if (isDir(f))
			if (!copyDir(f, toPath)) 
				return false;
		else
			if (!copySingleFile(f, toPath, false))
				return false;
	}

	return true;
}


bool KuPathUtil::copySingleFile(const std::string& filePath, const std::string& toPath, bool failIfExists)
{
	if (!exist(filePath) || isDir(filePath))
		return false;

	auto toPathFull = toPath;
	if (isDir(toPath) || hasEndingSeperator(toPath)) { // 构建完成路径
		toPathFull = makePath(toPath, fileName(filePath), "");
	}

	// TODO: posix支持
	return ::CopyFileA(filePath.c_str(), toPathFull.c_str(), failIfExists) != FALSE;
}


bool KuPathUtil::removeFile(const std::string& filePath)
{
	return ::_unlink(filePath.c_str()) == 0;
}


std::vector<std::string> KuPathUtil::getFiles(const std::string& path, bool includeDir, bool returnAbsPath)
{
	std::vector<std::string> files;
	
	auto fdir = isDir(path) ? path : parentPath(path); // 找到正在搜索的目录
	assureEndingSeperator(fdir); 

	_finddata_t info;
	intptr_t handle = _findfirst(path.c_str(), &info);
	if (handle != -1) {
		do {
			if (info.attrib & _A_SUBDIR) 
				if (std::strcmp(info.name, ".") == 0 ||
					std::strcmp(info.name, "..") == 0 ||
					!includeDir)
					continue;

			files.push_back(returnAbsPath ? fdir + info.name : info.name);

		} while (_findnext(handle, &info) == 0);

		_findclose(handle);
	}

	return files;
}


bool KuPathUtil::removeDir(const std::string& dir)
{
	auto subs = getFiles(dir, true, true);
	for (auto& s : subs) {
		if (isDir(s)) {
			if (!removeDir(s))
				return false;
		}
		else {
			if (!removeFile(s))
				return false;
		}
	}

	return ::_rmdir(dir.c_str()) == 0;
}


bool KuPathUtil::copyDir(const std::string& fromDir, const std::string& toDir)
{
	if (!exist(fromDir) || !isDir(fromDir))
		return false;

	return copyFiles(fromDir, join(toDir, fileName(fromDir)), true);
}


bool KuPathUtil::isSame(const std::string& path1, const std::string& path2)
{
	auto p1 = toAbs(path1);
	auto p2 = toAbs(path2);

	return ::_stricmp(path1.c_str(), path2.c_str()) == 0;
}


std::string KuPathUtil::pathFromHandle(FILE* f)
{
	HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(f));

	char path[MAX_PATH];
	GetFinalPathNameByHandleA(hFile, path, MAX_PATH, VOLUME_NAME_DOS); // TODO: posix支持

	return path;
}

