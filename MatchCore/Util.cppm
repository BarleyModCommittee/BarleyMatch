export module Util;

import index;
import <direct.h>;
import <windows.h>;
import <string>;

std::string working_dir;
bool init_complete = false;

/// @brief 获取文件夹在初始工作目录下的详细目录名
/// @param str 相对目录名，不含 './' 部分
/// @return 完整的绝对目录名称
export std::string GetWorkingDirName(std::string str);

std::string GetWorkingDirName(std::string str)
{
	if (!init_complete)
	{
		init_complete = true;
		
		char szFilePath[666] = { 0 };
		// 获取当前可执行文件的完整路径
		GetModuleFileNameA(NULL, szFilePath, 666);
		// 去掉文件名，仅保留目录路径
		(std::strrchr(szFilePath, '\\'))[0] = '\0';
		working_dir = szFilePath;
	}

	return working_dir + '\\' + str;
}