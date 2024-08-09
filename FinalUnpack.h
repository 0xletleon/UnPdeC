﻿// FinalUnpack.h
#ifndef FINAL_UNPACK_H
#define FINAL_UNPACK_H

#include "Common.h"
#include "UFunc.h"

namespace UnPdeC {

	class FinalUnpack {
	private:
		/// <summary>
		/// 位移表
		/// </summary>
		static unsigned int ByteLimit[16];

		/// <summary>
		/// 二次解密方法
		/// </summary>
		/// <param name="DeTempFileByte">需要解密的数据</param>
		/// <param name="">解码后的数据大小</param>
		/// <returns></returns>
		static std::vector<uint8_t> FinalDecrypt2(const std::vector<uint8_t>& DeTempFileByte, uint32_t& decryptedSize);
	public:
		/// <summary>
		/// 二次解密预处理
		/// </summary>
		/// <param name="DeTempFileByte">待解密的原始数据</param>
		/// <param name="FileName">文件名</param>
		/// <returns>解密后的数据</returns>
		static std::vector<uint8_t> PreDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName);
	};

} // namespace UnPdeC

#endif // FINAL_UNPACK_H