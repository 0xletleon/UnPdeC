// PdeTool.h
#ifndef PDETOOL_H
#define PDETOOL_H

#include "Common.h"
#include "GV.h"
#include "PdeKey.h"
#include "NameValidator.h"

namespace UnPdeC {

	/// <summary>
	/// PDE工具类
	/// </summary>
	class PdeTool {
	public:
		/// <summary>
		/// 初始化PDE工具类
		/// </summary>
		static void Init();

		/// <summary>
		/// 获取文件或文件夹偏移信息
		/// </summary>
		/// <param name="data">初次解密后的数据</param>
		/// <param name="BlockOffset">数据块在PDE文件中的偏移值</param>
		/// <returns>文件或文件夹的偏移信息数组 </returns>
		static std::vector<HexOffsetInfo> GetOffsetInfo(const std::vector<unsigned char>& data, uint32_t BlockOffset);

		/// <summary>
		/// 从PDE文件中获取指定块数据
		/// </summary>
		/// <param name="Start">块在PDE文件中的起始偏移</param>
		/// <param name="Size">块大小</param>
		/// <returns>块数据</returns>
		static GetOffsetStr GetByteOfPde(uint32_t Start, uint32_t Size);

		/// <summary>
		/// 解密文件 或! 数据块
		/// </summary>
		/// <param name="OffsetArr">文件或数据块的偏移信息数组</param>
		/// <returns>解密后的字节数据</returns>
		static std::vector<unsigned char> DeFileOrBlock(const std::vector<unsigned char>& OffsetArr);
	};

} // namespace UnPdeC

#endif // PDETOOL_H