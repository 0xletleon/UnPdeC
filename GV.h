#pragma once

#include "Common.h"

namespace UnPdeC {
	// 全局变量类
	class GV {
	public:
		static bool NeedDebugPde; // 是否需要调试用的
		static bool NeedOffsetLog; // 是否需要记录偏移日志文件
		static bool NeedFindDirs; // 是否需要查找未解码的目录
		static TNowPde NowPde; // 当前正在处理的PDE文件名
		static std::vector<unsigned char> PdeKey; // 解密密钥
		static std::filesystem::path ExeDir; // 当前程序所在目录
		static nlohmann::json OffsetLogResult; // 偏移日志结果
		static uint32_t NowExpandSize; // 当前文件扩充的大小
	};
}