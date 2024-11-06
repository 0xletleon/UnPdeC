#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <format>

namespace UnPdeC {
	class Encrypt {
	public:
		static std::vector<uint8_t> PreEncrypt(const std::vector<uint8_t>& originalData, bool useFlag6F = true);
		static uint32_t Put4Byte(std::vector<uint8_t>& data, size_t offset, uint32_t value);
		static uint32_t Get4Byte(const std::vector<uint8_t>& data, size_t offset);
		static std::vector<uint8_t> FinalEncrypt2(const std::vector<uint8_t>& originalData);

	private:
		static constexpr uint8_t ByteLimit[16] = { 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };
		static std::pair<uint32_t, uint32_t> findLongestMatch(const std::vector<uint8_t>& data, size_t currentPos);
		static void encodeMatch(std::vector<uint8_t>& output, uint32_t offset, uint32_t length);
		//static void encodeLiteral(std::vector<uint8_t>& output, uint8_t value);
	};
}