// FindPde.h
#ifndef FINDPDE_H
#define FINDPDE_H

#include "Common.h"
#include "GV.h"
#include <windows.h>

namespace UnPdeC {

	/// <summary>
	/// 查找PDE文件
	/// </summary>
	class FindPde {
	public:
		/// <summary>
		/// 查找当前目录下所有的.pde文件
		/// </summary>
		/// <returns> .pde文件名列表 </returns>
		static vector<TNowPde> Get();
	};
}

#endif // FINDPDE_H