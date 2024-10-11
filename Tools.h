#pragma once

#include "Common.h"
#include "GV.h"
#include "Decrypt.h"

namespace UnPdeC {
	/// <summary>
	/// PDE工具类
	/// </summary>
	class Tools {
	public:
		/// <summary>
		/// 初始化PDE工具类
		/// </summary>
		static void Init();

		/// <summary>
		/// 查找当前目录下所有的.xor文件
		/// </summary>
		/// <returns> .xor文件名列表 </returns>
		static std::vector<TNowXor> FindXorFile();

		// 将16进制转换为16进制字符串 HEX -> "0x1234"
		static std::string ToHexString(uint64_t value);

		/// <summary>
		/// 原始偏移值 -> 实际偏移值
		/// </summary>
		/// <param name="OrigOffset">原始偏移值</param>
		/// <returns>实际偏移值</returns>
		static uint64_t OrigToPdeOffset(uint64_t OrigOffset);

		/// <summary>
		/// 实际偏移值 -> 原始偏移值
		/// </summary>
		/// <param name="PdeOffset">实际偏移值</param>
		/// <returns>原始偏移值</returns>
		static uint64_t PdeToOrigOffset(uint64_t PdeOffset);

		/// <summary>
		/// 名称验证
		/// </summary>
		/// <param name="name">名称</param>
		/// <returns>是否合法</returns>
		static bool NameCheck(const std::string& name);

		/// <summary>
		/// 从xor文件中获取指定块数据
		/// </summary>
		/// <param name="Start">块在xor文件中的起始偏移</param>
		/// <param name="Size">块大小</param>
		/// <returns>块数据</returns>
		static GetOffsetStr GetByteOfXor(uint64_t Start, uint64_t Size);
	};
}