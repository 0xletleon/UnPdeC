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
	/// ���������࣬������PDE�ļ��в��Һʹ�����ܵ��ļ���Ŀ¼��Ϣ
	/// </summary>
	class ExtraSearch {
	public:
		/// <summary>
		/// ����������PDE�ļ��еĶ�����Ϣ
		/// </summary>
		static void SearchAndDecrypt();

	private:
		static const size_t BLOCK_SIZE = 1024 * 1024 * 10; // 10MB blocks
		static const std::vector<uint32_t> encryptedPatterns;

		/// <summary>
		/// �����ļ���һ����
		/// </summary>
		/// <param name="filePath">PDE�ļ�·��</param>
		/// <param name="startOffset">�����ʼƫ��</param>
		/// <param name="endOffset">��Ľ���ƫ��</param>
		static void ProcessFileBlock(const std::string& filePath, size_t startOffset, size_t endOffset);

		/// <summary>
		/// �ڿ�����������ģʽ
		/// </summary>
		/// <param name="block">Ҫ���������ݿ�</param>
		/// <param name="blockOffset">�����ļ��е�ƫ��</param>
		static void SearchInBlock(const std::vector<uint8_t>& block, size_t blockOffset);

		/// <summary>
		/// �����ҵ�����Ŀ
		/// </summary>
		/// <param name="info">��Ŀ��Ϣ</param>
		static void ProcessEntry(const HexOffsetInfo& info);

		/// <summary>
		/// ��ӡ����ģʽ
		/// </summary>
		static void PrintEncryptedPatterns();

		/// <summary>
		/// ��ӡ�ļ�ͷ
		/// </summary>
		static void PrintFileHeader();

		/// <summary>
		/// ִ�м򵥵���������
		/// </summary>
		static void SimpleLinearSearch();

		static std::mutex resultMutex;
		static std::vector<HexOffsetInfo> globalResults;
		static std::atomic<bool> shouldStop;
	};

}