#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace UnCache {

	class Decrypt {
	private:
		/// <summary>
		/// 位移表
		/// </summary>
		static uint8_t ByteLimit[16];

		/// <summary>
		///  获取4字节数据
		/// </summary>
		/// <param name="data"> 数据</param>
		/// <param name="offset"> 偏移</param>
		/// <returns> 4字节数据</returns>
		static std::uint32_t Get4Byte(const std::vector<std::uint8_t>& data, std::uint64_t offset);

		/// <summary>
		/// 二次解密方法
		/// </summary>
		/// <param name="DeTempFileByte">需要解密的数据</param>
		/// <param name="">解码后的数据大小</param>
		/// <returns></returns>
		static std::vector<uint8_t> FinalDecrypt2(const std::vector<uint8_t>& DeTempFileByte, uint64_t decryptedSize, const std::string& FileName);
	public:
		/// <summary>
		/// 二次解密预处理
		/// </summary>
		/// <param name="DeTempFileByte">待解密的原始数据</param>
		/// <param name="FileName">文件名</param>
		/// <returns>解密后的数据</returns>
		static std::vector<uint8_t> PreDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName);
	};

} 