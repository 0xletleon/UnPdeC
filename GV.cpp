// GV.cpp
#include "GV.h"

namespace UnPdeC {
	// 全局变量是否需要调试
	bool GV::NeedDebugPde = false;
	// 是否需要生成调试信息
	bool GV::NeedOffsetLog = false;
	// 是否需要生成解密后的PDE代码
	bool GV::NeedFindDirs = false;
	// 当前正在处理的PDE
	TNowPde GV::NowPde = { "", 0L };
	// 解密密钥
	vector<unsigned char> GV::PdeKey = vector<unsigned char>(1000, 0);
	// 当前程序所在目录
	std::filesystem::path GV::ExeDir = "";
	// 偏移日志结果
	nlohmann::json GV::OffsetLogResult;
}