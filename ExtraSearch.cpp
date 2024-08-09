// ExtraSearch.cpp
#include "ExtraSearch.h"
#include <fstream>

namespace UnPdeC {

	///// <summary>
	///// FC����ģʽ�б�
	///// </summary>
	//const std::vector<uint32_t> ExtraSearch::encryptedPatterns = {
	//	0x1C88154E, 0xAED655C9, 0xC743FFFE, 0x7D4F6172,
	//	0xBC5066E5, 0xBE0BA759, 0xBE0264C0, 0xB6B4ACEB,
	//	0xCF6B0908, 0x927D452A, 0x5562DAAB, 0xFA98B6D0,
	//	0xECAD839E, 0x66F08AD8, 0x3BA3FFC6, 0x9EBF09FE,
	//	0x7DC25EC3, 0x55B880C8, 0xABA5C923, 0xB66DBBF1,
	//	0x9BAADC93, 0xE285FDFE, 0xC56C5A33, 0x3BE6A209,
	//	0x0371239F, 0x6F939FB7, 0x4C08AC1E, 0x4E02E011,
	//	0xE56BA96B, 0x39DEF0BB, 0x7799455C, 0x856D7439
	//};

	/// <summary>
	/// AS����ģʽ�б�
	/// </summary>
	const std::vector<uint32_t> ExtraSearch::encryptedPatterns = {
		0xEA33174E, 0x586D57C9, 0x31F8FDFE, 0x8BF46372,
		0x4AEB64E5, 0x48B0A559, 0x48B966C0, 0x400FAEEB,
		0x39D00B08, 0x64C6472A, 0xA3D9D8AB, 0x0C23B4D0,
		0x1A16819E, 0x904B88D8, 0xCD18FDC6, 0x68040BFE,
		0x8B795CC3, 0xA30382C8, 0x5D1ECB23, 0x40D6B9F1,
		0x6D11DE93, 0x143EFFFE, 0x33D75833, 0xCD5DA009,
		0xF5CA219F, 0x99289DB7, 0xBAB3AE1E, 0xB8B9E211,
		0x13D0AB6B, 0xCF65F2BB, 0x8122475C, 0x73D67639
	};

	std::mutex ExtraSearch::resultMutex;
	std::vector<HexOffsetInfo> ExtraSearch::globalResults;
	std::atomic<bool> ExtraSearch::shouldStop(false);

	void ExtraSearch::SearchAndDecrypt() {
		cout << "��ʼ�����ͽ��ܹ���..." << endl;
		PrintEncryptedPatterns();
		PrintFileHeader();

		std::ifstream file(GV::NowPde.Path, std::ios::binary | std::ios::ate);
		if (!file) {
			throw std::runtime_error("Unable to open PDE file: " + GV::NowPde.Path);
		}

		size_t fileSize = file.tellg();
		file.close();
		cout << "PDE�ļ���С: " << fileSize << " �ֽ�" << endl;

		std::vector<std::thread> threads;
		size_t numThreads = std::thread::hardware_concurrency();
		cout << "ʹ�� " << numThreads << " ���߳̽�������" << endl;

		for (size_t i = 0; i < fileSize; i += BLOCK_SIZE) {
			size_t endOffset = std::min(i + BLOCK_SIZE, fileSize);
			cout << "�����ļ���: " << i << " �� " << endOffset << endl;
			threads.emplace_back(&ExtraSearch::ProcessFileBlock, GV::NowPde.Path, i, endOffset);
			
			if (threads.size() >= numThreads) {
				for (auto& t : threads) {
					t.join();
				}
				threads.clear();
			}
		}

		for (auto& t : threads) {
			t.join();
		}

		cout << "������ɣ��ҵ� " << globalResults.size() << " �����" << endl;

		SimpleLinearSearch();

		for (const auto& info : globalResults) {
			ProcessEntry(info);
		}

		cout << "�������" << endl;
	}

	void ExtraSearch::ProcessFileBlock(const std::string& filePath, size_t startOffset, size_t endOffset) {
		cout << "�����ļ���: " << startOffset << " �� " << endOffset << endl;
		std::ifstream file(filePath, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Unable to open PDE file: " + filePath);
		}

		file.seekg(startOffset);
		std::vector<uint8_t> block(endOffset - startOffset);
		file.read(reinterpret_cast<char*>(block.data()), block.size());

		SearchInBlock(block, startOffset);
	}

	void ExtraSearch::SearchInBlock(const std::vector<uint8_t>& block, size_t blockOffset) {
		cout << "��ƫ�� " << blockOffset << " ��������" << endl;
		for (size_t i = 0; i < block.size() - 3; ++i) {
			if (shouldStop) return;

			uint32_t currentValue = *reinterpret_cast<const uint32_t*>(&block[i]);

			for (size_t j = 0; j < encryptedPatterns.size(); ++j) {
				if ((currentValue & 0xFFFFFF00) == (encryptedPatterns[j] & 0xFFFFFF00)) {
					cout << "��ƫ�� " << (blockOffset + i) << " ���ҵ�Ǳ��ƥ��: 0x"
						<< std::hex << currentValue << std::dec
						<< " (Pattern " << j << ")" << endl;

					HexOffsetInfo info;
					info.OriginalOffset = blockOffset + i - 0x74;
					info.PatternIndex = j;

					std::lock_guard<std::mutex> lock(resultMutex);
					globalResults.push_back(info);
				}
			}
		}
	}

	void ExtraSearch::ProcessEntry(const HexOffsetInfo& info) {
		cout << "������Ŀ��ԭʼƫ��: " << info.OriginalOffset << endl;

		std::ifstream file(GV::NowPde.Path, std::ios::binary);
		if (!file) {
			cout << "�޷���PDE�ļ����н���" << endl;
			return;
		}

		file.seekg(info.OriginalOffset);
		std::vector<uint8_t> encryptedData(0x80);
		file.read(reinterpret_cast<char*>(encryptedData.data()), 0x80);

		cout << "��������: ";
		for (int i = 0; i < 16; ++i) {
			cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(encryptedData[i]) << " ";
		}
		cout << std::dec << endl;

		std::vector<uint8_t> decryptedData(0x80);
		for (size_t i = 0; i < 0x80; ++i) {
			size_t keyIndex = (info.PatternIndex * 0x80 + i) % GV::PdeKey.size();
			decryptedData[i] = encryptedData[i] ^ GV::PdeKey[keyIndex];
		}

		cout << "��������: ";
		for (int i = 0; i < 16; ++i) {
			cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(decryptedData[i]) << " ";
		}
		cout << std::dec << endl;

		uint8_t type = decryptedData[0];
		std::string name;
		for (size_t i = 1; i < 0x74 && decryptedData[i] != 0; ++i) {
			name += static_cast<char>(decryptedData[i]);
		}

		union {
			uint32_t value;
			uint8_t bytes[4];
		} OriginalOffset{};
		std::copy(decryptedData.begin() + 0x78, decryptedData.begin() + 0x7C, OriginalOffset.bytes);

		uint32_t pdeOffset = ((OriginalOffset.value >> 10) + OriginalOffset.value + 1) << 12;
		uint32_t size = *reinterpret_cast<uint32_t*>(&decryptedData[0x7C]);

		cout << "����: " << (type == 1 ? "�ļ�" : "Ŀ¼") << endl;
		cout << "����: " << name << endl;
		cout << "ԭʼƫ��ֵ: 0x" << std::hex << OriginalOffset.value << std::dec << endl;
		cout << "PDEƫ��: " << pdeOffset << endl;
		cout << "��С: " << size << endl;

		if (type == 2) { // Ŀ¼
			bool exists = false;
			for (const auto& entry : GV::OffsetLogResult) {
				if (entry["Type"] == "Directory" && entry["Name"] == name) {
					exists = true;
					break;
				}
			}

			if (exists) {
				cout << "Ŀ¼�Ѵ���: " << name << endl;
			} else {
				cout << "������Ŀ¼: " << name << endl;
				nlohmann::json newEntry;
				newEntry["Type"] = "Directory";
				newEntry["Name"] = name;
				newEntry["PdeOffset"] = pdeOffset;
				newEntry["Size"] = size;
				newEntry["OriginalOffset"] = info.OriginalOffset;
				GV::OffsetLogResult.push_back(newEntry);
			}
		} else if (type == 1) { // �ļ�
			cout << "�����ļ�: " << name << endl;
		}
	}

	void ExtraSearch::PrintEncryptedPatterns() {
		cout << "����ģʽ: " << endl;
		for (size_t i = 0; i < encryptedPatterns.size(); ++i) {
			cout << "Pattern " << i << ": 0x" << std::hex << encryptedPatterns[i] << std::dec << endl;
		}
	}

	void ExtraSearch::PrintFileHeader() {
		std::ifstream file(GV::NowPde.Path, std::ios::binary);
		if (!file) {
			cout << "�޷����ļ�" << endl;
			return;
		}
		std::vector<uint8_t> header(256);
		file.read(reinterpret_cast<char*>(header.data()), header.size());
		cout << "�ļ�ͷ (ǰ256�ֽ�): " << endl;
		for (size_t i = 0; i < header.size(); ++i) {
			cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(header[i]) << " ";
			if ((i + 1) % 16 == 0) cout << endl;
		}
		cout << std::dec << endl;
	}

	void ExtraSearch::SimpleLinearSearch() {
		cout << "ִ�м���������..." << endl;
		std::ifstream file(GV::NowPde.Path, std::ios::binary);
		if (!file) {
			cout << "�޷����ļ�������������" << endl;
			return;
		}

		std::vector<uint8_t> buffer(4096);
		size_t totalOffset = 0;
		while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
			for (size_t i = 0; i < buffer.size() - 3; ++i) {
				uint32_t currentValue = *reinterpret_cast<uint32_t*>(&buffer[i]);
				for (size_t j = 0; j < encryptedPatterns.size(); ++j) {
					if ((currentValue & 0xFFFFFF00) == (encryptedPatterns[j] & 0xFFFFFF00)) {
						cout << "����������ƫ�� " << (totalOffset + i) << " ���ҵ�ƥ��: 0x"
							<< std::hex << currentValue << std::dec
							<< " (Pattern " << j << ")" << endl;
					}
				}
			}
			totalOffset += buffer.size();
		}
		cout << "�����������" << endl;
	}

} // namespace UnPdeC