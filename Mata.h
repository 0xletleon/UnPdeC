#pragma once

#include "Common.h"
#include "GV.h"
#include "Tools.h"
#include "Decrypt.h"

namespace UnPdeC {
	/// <summary>
	/// PDE工具类
	/// </summary>
	class Mata {
	public:
		/// <summary>
		/// 提取元信息
		/// </summary>
		/// <param name="MataBlock">块数据</param>
		/// <param name="XorFileSize">.xor文件大小</param>
		/// <returns>元信息</returns>
		static BlockInfo ExtractMataInfo(const uint8_t* MataBlock, uint64_t XorFileSize);

		/// <summary>
		/// 读取.xor中所有元数据块
		/// </summary>
		/// <param name="FilePath">.xor文件路径</param>
		/// <returns>所有文件/夹的元数据json</returns>
		static json ReadMatakForXorFile(const std::filesystem::path& FilePath);

		/// <summary>
		/// 解码并保存文件/夹
		/// </summary>
		/// <param name="MataJson">待保文件夹元信息</param>
		static void DecodeAndSaveFile(const MataSaveInfo& MataJson);

		/// <summary>
		/// 提取元信息并解码
		/// </summary>
		/// <param name="XorFileMatas">Xor文件元信息</param>
		static void ExtractMateAndDecode(const json& XorFileMatas);
	};
}