// GV.h
#ifndef GV_H
#define GV_H

#include "Common.h"
#include "FindPde.h"

namespace UnPdeC {
	/// <summary>
	/// 全局变量类
	/// </summary>
	class GV {
	public:
		// 是否需要调试用的
		static bool NeedDebugPde;
		// 是否需要记录偏移日志文件
		static bool NeedOffsetLog;
		// 是否需要查找未解码的目录
		static bool NeedFindDirs;
		// 当前正在处理的PDE文件名
		static TNowPde NowPde;
		// 解密密钥
		static vector<unsigned char> PdeKey;
	};
}
#endif // GV_H