#include "Encrypt.h"
#include <iostream>

namespace UnPdeC {
	uint32_t Encrypt::Put4Byte(std::vector<uint8_t>& data, size_t offset, uint32_t value) {
		if (offset + 4 > data.size()) {
			throw std::out_of_range("PdeOffset out of range");
		}
		std::memcpy(data.data() + offset, &value, sizeof(value));
		return value;
	}

	uint32_t Encrypt::Get4Byte(const std::vector<uint8_t>& data, size_t offset) {
		if (offset + 4 > data.size()) {
			throw std::out_of_range("PdeOffset out of range");
		}
		uint32_t value;
		std::memcpy(&value, data.data() + offset, sizeof(value));
		return value;
	}

std::vector<uint8_t> Encrypt::PreEncrypt(const std::vector<uint8_t>& luacData, bool useFlag6F) {
    try {
        // �Ȼ�ȡ���ܺ������
        auto encryptedData = FinalEncrypt2(luacData);

        // ������ִ�С
        uint32_t totalSize = static_cast<uint32_t>(luacData.size());          // ���ܺ�Ĵ�С
        uint32_t dataSize = static_cast<uint32_t>(encryptedData.size() + 0x21 - 0x18);  // ��0x18�������Ĵ�С
        uint32_t endSize = static_cast<uint32_t>(encryptedData.size());       // ��0x6F�������Ĵ�С
		// ���������������
		std::cout << "Encrypted Data Size: " << encryptedData.size() << std::endl;
		std::cout << "Encrypted Data Hex: " << std::endl;
		for (size_t i = 0; i < encryptedData.size(); i++) {
			printf("%02X", encryptedData[i]);
			if ((i + 1) % 16 == 0) std::cout << std::endl;
			else if ((i + 1) % 4 == 0) std::cout << " ";
		}
		std::cout << std::endl << std::endl;

		// �����С
		//uint32_t dataSize = static_cast<uint32_t>(encryptedData.size() + 0x21 - 0x18);
		uint32_t originalSize = static_cast<uint32_t>(luacData.size());

		// ���������С��Ϣ
		std::cout << "Original Size: 0x" << std::hex << originalSize << std::endl;
		std::cout << "Data Size: 0x" << std::hex << dataSize << std::endl;
		std::cout << "Encrypted Size: 0x" << std::hex << encryptedData.size() << std::endl;


        // �����ļ�ͷ
        std::vector<uint8_t> finalData;
        finalData.reserve(0x21 + encryptedData.size());

        // 1. �̶�ͷ�� (0x00-0x0B)
        const uint8_t fixedHeader[] = {
            0x01, 0x0A, 0x00, 0x00,  // Lua�ļ���ʶ
            0x02, 0x00, 0x01, 0x00,  // �汾��Ϣ
            0x00, 0x00, 0x00, 0x00   // Ԥ��
        };
        finalData.insert(finalData.end(), fixedHeader, fixedHeader + sizeof(fixedHeader));

        // 2. ���ݴ�С (0x0C-0x0F)
        finalData.resize(finalData.size() + 4);
        Put4Byte(finalData, 0x0C, dataSize);

        // 3. ��֤�� (0x10-0x13)
        const uint8_t verification[] = {
            0xFF, 0xFF, 0xFF, 0xFF  // ��֤��
        };
        finalData.insert(finalData.end(), verification, verification + sizeof(verification));

        // 4. �ļ����� (0x14-0x17)
        const uint8_t fileCount[] = {
            0x01, 0x00, 0x00, 0x00  // 1���ļ�
        };
        finalData.insert(finalData.end(), fileCount, fileCount + sizeof(fileCount));

        // 5. �ļ���ʶ (0x18)
        finalData.push_back(0x6F);

        // 6. ������С (0x19-0x1C)
        finalData.resize(finalData.size() + 4);
        Put4Byte(finalData, 0x19, dataSize);

        // 7. ���ܺ��С (0x1D-0x20)
        finalData.resize(finalData.size() + 4);
        Put4Byte(finalData, 0x1D, totalSize + 8);

        // 8. ��Ӽ��ܺ������
        finalData.insert(finalData.end(), encryptedData.begin(), encryptedData.end());

        return finalData;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::format("Encrypt::PreEncrypt() failed: {}", e.what()));
    }
}

	std::vector<uint8_t> Encrypt::FinalEncrypt2(const std::vector<uint8_t>& data) {
		try {
			std::vector<uint8_t> encryptedData;
			encryptedData.reserve(data.size() * 2);  // Ԥ����ռ�

			size_t position = 0;
			uint32_t controlBits = 1;
			size_t controlPos = 0;

			// Ԥ���׸������ֵ�λ��
			encryptedData.resize(4, 0);

			while (position < data.size()) {
				if (controlBits == 1) {
					// ��Ҫ�µĿ�����
					controlPos = encryptedData.size();
					encryptedData.resize(controlPos + 4);
					controlBits = 0x80000000;
				}

				// �����ظ�ģʽ
				auto [offset, length] = findLongestMatch(data, position);

				if (length >= 3) {
					// д���ظ�ģʽ
					encodeMatch(encryptedData, offset, length);
					position += length;
					// ���ÿ���λ
					Put4Byte(encryptedData, controlPos, Get4Byte(encryptedData, controlPos) | controlBits);
				} else {
					// ֱ��д���ֽ�
					encryptedData.push_back(data[position]);
					position++;
				}

				controlBits >>= 1;
			}

			return encryptedData;
		} catch (const std::exception& e) {
			throw std::runtime_error(std::format("Encrypt::FinalEncrypt2() failed: {}", e.what()));
		}
	}

	std::pair<uint32_t, uint32_t> Encrypt::findLongestMatch(const std::vector<uint8_t>& data, size_t currentPos) {
		if (currentPos < 3) return { 0, 0 };

		uint32_t bestOffset = 0;
		uint32_t bestLength = 0;

		const size_t windowSize = std::min(currentPos, size_t(0x1FFFF));
		const size_t searchStart = currentPos - windowSize;
		const size_t maxLength = std::min(0xFF + 3u, static_cast<unsigned int>(data.size() - currentPos));

		for (size_t i = searchStart; i < currentPos; ++i) {
			size_t currentLength = 0;
			while (currentLength < maxLength &&
				   data[i + currentLength] == data[currentPos + currentLength]) {
				currentLength++;
			}

			if (currentLength >= 3 && currentLength > bestLength) {
				bestOffset = static_cast<uint32_t>(currentPos - i);
				bestLength = static_cast<uint32_t>(currentLength);
			}
		}

		return { bestOffset, bestLength };
	}

	void Encrypt::encodeMatch(std::vector<uint8_t>& output, uint32_t offset, uint32_t length) {
		if (length <= 0x1F + 2 && offset <= 0x3FF) {
			// 2�ֽڱ���
			uint32_t encodedValue = ((offset & 0x3FF) << 6) | ((length - 2) << 2) | 2;
			output.push_back(static_cast<uint8_t>(encodedValue & 0xFF));
			output.push_back(static_cast<uint8_t>((encodedValue >> 8) & 0xFF));
		} else if (length <= 0xFF + 3) {
			// 3�ֽڱ���
			uint32_t encodedValue = (offset << 15) | ((length - 3) << 7) | 3;
			output.push_back(static_cast<uint8_t>(encodedValue & 0xFF));
			output.push_back(static_cast<uint8_t>((encodedValue >> 8) & 0xFF));
			output.push_back(static_cast<uint8_t>((encodedValue >> 16) & 0xFF));
		} else {
			throw std::runtime_error("Match length too long");
		}
	}
}