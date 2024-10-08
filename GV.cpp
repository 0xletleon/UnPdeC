// GV.cpp
#include "GV.h"

namespace UnPdeC {
	// 是否需要调试文件
	bool GV::NeedDebugPde = true;
	// 是否需要生成偏移记录
	bool GV::NeedOffsetLog = true;
	// 是否需要生查找未知文件或目录
	bool GV::NeedFindDirs = true;
	// 当前正在处理的PDE
	TNowPde GV::NowPde = { "", 0L };
	// 解密密钥
	vector<unsigned char> GV::PdeKey = vector<unsigned char>(1000, 0);
	// 当前程序所在目录
	std::filesystem::path GV::ExeDir = "";
	// 偏移日志结果
	nlohmann::json GV::OffsetLogResult;
	// 当前文件扩充的大小
	uint32_t GV::NowExpandSize = 0x0;
}