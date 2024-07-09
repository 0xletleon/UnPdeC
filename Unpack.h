// Unpack.h
#ifndef UNPACK_H
#define UNPACK_H

#include "Common.h"
#include "PdeTool.h"
#include "FinalUnpack.h"
#include "OffsetLog.h"

namespace UnPdeC {
	class Unpack {
	private:
		/// <summary>
		/// 暂时不参与二次解密的文件
		/// </summary>
		static const vector<string> PassArr;
		/// <summary>
		/// 查找后缀
		/// </summary>
		/// <param name="target"> 文件名 </param>
		/// <returns> 是否存在后缀 </returns>
		static bool FindSuffix(const std::string& target);

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

} // namespace UnPdeC

#endif // UNPACK_H