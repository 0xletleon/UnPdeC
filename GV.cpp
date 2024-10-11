#include "GV.h"

namespace UnPdeC {
	// 当前正在处理的Xor文件
	TNowXor GV::NowXor = { "", 0L };
	// 当前程序所在目录
	std::filesystem::path GV::ExeDir = "";
}