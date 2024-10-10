#pragma once

#include "Common.h"

namespace UnPdeC {
	/// <summary>
	/// 实用方法类
	/// </summary>
	class UFunc {
	public:
		/// <summary>
		///  获取4字节数据
		/// </summary>
		/// <param name="data"> 数据</param>
		/// <param name="offset"> 偏移</param>
		/// <returns> 4字节数据</returns>
		static std::uint32_t Get4Byte(const std::vector<std::uint8_t>& data, std::uint64_t offset);
	};
}
