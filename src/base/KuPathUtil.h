#pragma once
#include <vector>
#include <string>


// 文件路径支持类
// 成员函数可分为两类：
//   一是字符串层面的操作，不涉及实际的文件系统，此类函数放在类的前面声明
//   二是文件系统层面操作，主要调用操作系统底层接口实现
class KuPathUtil
{
public:
	constexpr static char kWinSep = '\\';
	constexpr static char kUnixSep = '/';
	constexpr static char kSeps[] = { kWinSep, kUnixSep };

#if defined(WIN32)
	constexpr static char nativeSep = kWinSep;
#else
	constexpr static char nativeSep = kUnixSep;
#endif

	// 暂时支持正斜杠、反斜杠两类分隔符
	static bool isSeperator(char ch) { return std::strchr(kSeps, ch) != nullptr; }


	// 规范化路径分割符，共两步操作：
	// 一是替换，即将path中的分隔符替换为nativeSep
	// 二是折叠，即将连续出现的多个分隔符折叠为单个分隔符，如"///"折叠为"/"
	static std::string normSeps(const std::string& path, char normSep = nativeSep);

	static std::string toWinStyle(const std::string& path) { return normSeps(path, kWinSep); }
	static std::string toUnixStyle(const std::string& path) { return normSeps(path, kUnixSep); }

	// path是否以分隔符结尾
	static bool hasEndingSeperator(const std::string& path);
	static void trimEndingSeperator(std::string& path);
	static void assureEndingSeperator(std::string& path, char sep = nativeSep);


	// 规范化路径，共三步操作：
	// 一是规范化分隔符
	// 二是解析相"../"和"./"
	// 三是裁剪结尾的空格和点号字符
	static std::string norm(const std::string& path, char normSep = nativeSep);


	// 提取路径中的根，共有4种类型：
	// 一是绝对盘符，如c:\
	// 二是相对盘符，如c:
    // 三是根目录，即\
	// 四是UNC根节点，如\\server\c$、\\?\c:
	// 对于c:\foo.txt，返回c:\
	// 对于c:foo.txt，返回c:
    // 对于\foo.txt，返回\
	// 对于UNC路径，返回hostname+sharename，兼容DOS设备路径和windows长路径名
	// 例如：\\server\c$\foo.txt，返回\\server\c$
	//       \\?\c:\foo.txt，返回\\?\c:\
	//       \\.\Volume{b75e2c83-0000-0000-0000-602f00000000}\foo.txt，返回\\.\Volume{b75e2c83-0000-0000-0000-602f00000000}\
	//       \\.\UNC\server\share\foo.txt，返回\\.\UNC\server\share
	static std::string splitRoot(const std::string& path);

	// Returns a value that indicates whether a file path contains a root.
	// Note: rooted paths can be either absolute (that is, fully qualified) or relative.
	static bool isRooted(const std::string& path) {
		return !splitRoot(path).empty();
	}

	static bool isAbsRoot(const std::string& root) {
		return root.size() > 2; // 排除"c:"和"/"两种情况，字符串长度均不大于2
	}

	// return true if the path is fixed to a specific drive or UNC path; 
	// false if the path is relative to the current drive or working directory.
	static bool isAbsPath(const std::string& path) {
		return isAbsRoot(splitRoot(path)); 
	}


	// 对root进行规范化，返回归一的hostname和sharename
	// 对于非UNC路径，hostname返回空；
	// 对于UNC路径，"."、"?"、127.0.0.1"和"localhost"等hostname统一规范化为空，其他hostname保持不变
	// 对于UNC路径，"c$\"等sharename规范化为"c:\"
	static std::pair<std::string, std::string> normRoot(const std::string& root);


	// 判断两个根路径是否相同
	static bool isSameRoot(const std::string& root1, const std::string& root2);


	// 用分隔符将s1、s2、s3、s4等各字符串连接为路径
	// NOTE: This method assumes that the first argument is an absolute path 
	// and that the following argument or arguments are relative paths.
	// However, if an argument other than the first contains a rooted path, 
	// any previous path components are ignored, and the returned string 
	// begins with that rooted path component. 
	static std::string combine(const std::string& s1, const std::string& s2) {
		return isRooted(s2) ? s2 : join(s1, s2);
	}

	static std::string combine(const std::string& s1, const std::string& s2, const std::string& s3) {
		return combine(combine(s1, s2), s3);
	}

	static std::string combine(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4) {
		return combine(combine(s1, s2, s3), s4);
	}


	// 与combine功能相似，但不做root检测，直接用分隔符连接字符串
	static std::string join(const std::string& s1, const std::string& s2);

	static std::string join(const std::string& s1, const std::string& s2, const std::string& s3) {
		return join(join(s1, s2), s3);
	}

	static std::string join(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4) {
		return join(join(s1, s2, s3), s4);
	}


	// 带文件后缀名的路径字符串合成，只构建路径字符串，不创建文件
	static std::string makePath(const std::string& dir, const std::string& fileBaseName, const std::string& fileExt);


	// 返回fullPath基于parentPath的相对路径，可当作fullPath - parentPath，或者combine的逆操作
	// 如果fullPath与parentPath的根路径不同，则返回fullPath
	static std::string getRelPath(const std::string& fullPath, const std::string& parentPath);


	// 从path分裂出路径的第一部分
	// @sep：路径分割符，若sep=0，则kSeps中的字符均为分隔符
	static std::string splitFirstPart(std::string& path, char sep);

	// 从path分裂出路径的最后一部分
	// @sep：同splitFirstPart
	static std::string splitLastPart(std::string& path, char sep);


	// 将路径path分割为目录部分和文件名部分，返回{ dir, fileName }
	// 该实现不对路径进行规范化和解析操作，"../"和"./"都将保留
	// @fileName：结果可能是文件名，也可能是目录名，甚至可能是".."或"."
	// 比如："c:/dir/foo.txt"分割为"c:/dir"与"foo.txt"
	//       "c:/dir/test/"分割为"c:/dir"与"test"
	//       "c:/dir/../分割为"c:/dir"与".."
	static std::pair<std::string, std::string> splitPath(const std::string& path);

	// 将文件名分割为基础名字baseName和扩展名ext，返回{ baseName, ext }
	// @fullExt：若为true，则foo.txt.doc分割为foo与txt.doc，否则分割为foo.txt与doc
	static std::pair<std::string, std::string> splitFileName(const std::string& fileName, bool fullExt);


	// 一些快捷方法
	static std::string fileName(const std::string& path) {
		return splitPath(path).second;
	}

	static std::string baseName(const std::string& path, bool fullExt) {
		return splitFileName(fileName(path), fullExt).first;
	}

	static std::string extension(const std::string& path, bool fullExt) {
		return splitFileName(fileName(path), fullExt).second;
	}

	
	static bool isLegalFileName(const std::string& fileName); // check if strPath is a legal file name

	static bool isDosDeviceName(const std::string& fileName);



	/**************** 以下函数调用操作系统底层函数实现 ****************/

	// 当前路径获取与设置
	static std::string getCwd();
	static bool setCwd(const std::string& dir);


	// 路径是否存在
	static bool exist(const std::string& path);


	// 结合当前目录解析relpath，返回绝对路径
	static std::string toAbs(const std::string& relpath); 


	// 获取path的父路径
	static std::string parentPath(const std::string& path);


	static bool isSame(const std::string& path1, const std::string& path2);


	static std::string pathFromHandle(FILE* hf);


	static bool isDir(const std::string& path);


	static std::string browse(const std::string& path);


	// 拷贝单个文件
	// @fromFilePath，源路径，若不存在或者为目录，返回false
	// @toFilePath，目标路径，若是已存在目录，或以'\\'或'/'结尾，则当成目录看待，否则当成文件路径看待
	// @failIfExists，若为true，则当toFilePath为已存在文件时，不执行拷贝操作，返回false
	static bool copySingleFile(const std::string& fromFilePath, const std::string& toFilePath, bool failIfExists);

	// 将目录fromPath中的文件拷贝到目录toPath
	// 若toPath不存在，则创建该目录
	// 若toPath中存在同名文件，则直接覆盖
	static bool copyFiles(const std::string& fromPath, const std::string& toPath, bool includeSubDirs = true);

	// 拷贝整个目录
	// "copy c:\aa\bb\cc d:\ee" will copy 'cc' sub-directory of c:\aa\bb\ to d:\ee\cc
	static bool copyDir(const std::string& fromDir, const std::string& toDir);
	
	// 删除单个文件
	static bool removeFile(const std::string& filePath);

	// 删除目录dir，及包含的所有子目录和文件
	static bool removeDir(const std::string& dir);

	static bool createDir(const std::string& path);

	// 使用_findfirst函数实现. path支持通配符
	static std::vector<std::string> getFiles(const std::string& path, bool includeDir, bool returnAbsPath);


private:
	KuPathUtil(void) {}
	~KuPathUtil(void) {}
};

