﻿#pragma once

#include "Common.h"

namespace UnPdeC {
	// 全局变量类
	class GV {
	public:
		static TNowXor NowXor; // 当前正在处理的Xor文件名
		static std::filesystem::path ExeDir; // 当前程序所在目录
		static bool CacheMode; // 不二次解压,输出一次解压后的.cache文件
	};
}