#pragma once

#include "Common.h"
#include "GV.h"
#include "NameValidator.h"
#include "FinalUnpack.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <iomanip>

namespace UnPdeC {

	/// <summary>
	/// 额外搜索类，用于在PDE文件中查找和处理加密的文件和目录信息
	/// </summary>
	class ExtraSearch {
	public:
		/// <summary>
		/// 搜索并解密PDE文件中的额外信息
		/// </summary>
		static void SearchAndDecrypt();

	private:
		static const size_t BLOCK_SIZE = 1024 * 1024 * 10; // 10MB blocks
		static const std::vector<uint32_t> encryptedPatterns;

		/// <summary>
		/// 处理文件的一个块
		/// </summary>
		/// <param name="filePath">PDE文件路径</param>
		/// <param name="startOffset">块的起始偏移</param>
		/// <param name="endOffset">块的结束偏移</param>
		static void ProcessFileBlock(const std::string& filePath, size_t startOffset, size_t endOffset);

		/// <summary>
		/// 在块中搜索加密模式
		/// </summary>
		/// <param name="block">要搜索的数据块</param>
		/// <param name="blockOffset">块在文件中的偏移</param>
		static void SearchInBlock(const std::vector<uint8_t>& block, size_t blockOffset);

		/// <summary>
		/// 处理找到的条目
		/// </summary>
		/// <param name="info">条目信息</param>
		static void ProcessEntry(const HexOffsetInfo& info);

		/// <summary>
		/// 打印加密模式
		/// </summary>
		static void PrintEncryptedPatterns();

		/// <summary>
		/// 打印文件头
		/// </summary>
		static void PrintFileHeader();

		/// <summary>
		/// 执行简单的线性搜索
		/// </summary>
		static void SimpleLinearSearch();

		static std::mutex resultMutex;
		static std::vector<HexOffsetInfo> globalResults;
		static std::atomic<bool> shouldStop;
	};

}