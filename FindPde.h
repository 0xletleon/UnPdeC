#pragma once

#include "Common.h"
#include "GV.h"
//#include <windows.h>

namespace UnPdeC {

	// 查找PDE文件
	class FindPde {
	public:
		/// <summary>
		/// 查找当前目录下所有的.pde文件
		/// </summary>
		/// <returns> .pde文件名列表 </returns>
		static std::vector<TNowPde> Get();
	};
}