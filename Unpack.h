#pragma once

#include "Common.h"
#include "PdeTool.h"
#include "FinalUnpack.h"
#include "OffsetLog.h"

namespace UnPdeC {
	class Unpack {
	private:
	public:
		/// <summary>
		/// 尝试解密
		/// </summary>
		/// <param name="PdeOffset">数据块在PDE文件中的偏移值</param>
		/// <param name="Size">数据块大小</param>
		/// <param name="Dir">目录</param>
		/// <param name="Is170">是否为170表数据</param>
		static void Try(uint32_t PdeOffset, uint32_t Size, const DirStr& Dir, bool Is170);

		/// <summary>
		/// 创建目录或文件
		/// </summary>
		/// <param name="DirOrFileArr">文件或目录数组</param>
		/// <param name="Dir">目录</param>
		/// <param name="BlockOffset">数据块在PDE文件中的偏移值</param>
		static void Save(const vector<HexOffsetInfo>& DirOrFileArr, const DirStr& Dir, uint32_t BlockOffset);
	};

}