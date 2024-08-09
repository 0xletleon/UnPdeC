// UFunc.cpp
#ifndef UFUNC_CPP
#define UFUNC_CPP

#include "UFunc.h"

namespace UnPdeC {
	//std::uint32_t UFunc::Get4Byte(const std::vector<std::uint8_t>& data, std::uint32_t offset) {
	//	// 测试处理offset越界问题
	//	if (offset > 0xFF000000) {
	//		offset = ~offset;
	//	}
	//	// 结果
	//	std::uint32_t result = 0;

	//	// 检查是否越界
	//	if (offset < data.size()) {
	//		// 计算剩余字节数
	//		std::size_t remaining = data.size() - offset;
	//		// 显式指定 std::min 的模板参数类型
	//		std::size_t bytesToCopy = std::min<std::size_t>(remaining, static_cast<std::size_t>(4u));

	//		// 将剩余字节复制到result中，并转换为小端字节序
	//		for (std::size_t i = 0; i < bytesToCopy; ++i) {
	//			result |= (data[offset + i] << (i * 8));
	//		}
	//		//// 计算剩余字节数
	//		//std::size_t remaining = data.size() - offset;
	//		//// 显式指定 std::min 的模板参数类型
	//		//std::size_t bytesToCopy = std::min<std::size_t>(remaining, static_cast<std::size_t>(4u));

	//		//// 将剩余字节复制到result中
	//		//for (std::size_t i = 0; i < bytesToCopy; ++i) {
	//		//	result |= (data[offset + i] << (24 - 8 * i)); // 注意这里是24，不是8
	//		//}

	//		//// 如果剩余字节不足4个，则将高位补0
	//		//for (std::size_t i = bytesToCopy; i < 4; ++i) {
	//		//	result <<= 8; // 左移8位，相当于补0
	//		//}
	//	} else {
	//		// 处理越界错误
	//		std::cout << "越界";
	//		result = { 0x00000000 };
	//	}
	//	// 返回结果
	//	return result;
	//}

	std::uint32_t UFunc::Get4Byte(const std::vector<std::uint8_t>& data, std::uint32_t offset) {
		if (static_cast<unsigned long long>(offset) + 4 > data.size()) {
			throw std::out_of_range("PdeOffset out of range");
		}
		std::uint32_t value;
		std::memcpy(&value, data.data() + offset, sizeof(value));
		return value;
	}
}

#endif // UFUNC_CPP