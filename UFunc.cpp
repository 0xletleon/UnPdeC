// UFunc.cpp
#ifndef UFUNC_CPP
#define UFUNC_CPP

#include "UFunc.h"

namespace UnPdeC {
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