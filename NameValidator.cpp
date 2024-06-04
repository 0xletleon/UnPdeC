// NameValidator.cpp
#include "NameValidator.h"

namespace UnPdeC {
	bool NameValidator::Check(uint8_t Type, const string& Name) {
		if (Type == 1) {
			// 判断文件名是否合法
			return IsValidName(Name, false);
		} else if (Type == 2) {
			// 判断目录名是否合法
			return IsValidName(Name, true);
		}
		return false;
	}

	/// <summary>
	///  检查字符是否合法
	/// </summary>
	/// <param name="c"> 字符 </param>
	/// <returns> 是否合法 </returns>
	bool NameValidator::IsValidChar(char c) {
		return c == '_' || c == '-' || c == '\\' || c == '"' || c == '.' ||
			std::isdigit(c) || std::isalpha(c);
	}

	/// <summary>
	/// 检查文件/目录名是否合法
	/// </summary>
	/// <param name="name"> 文件名或目录名 </param>
	/// <param name="isDirectory"> 是否为目录 </param>
	/// <returns></returns>
	bool NameValidator::IsValidName(const std::string& name, bool isDirectory) {
		if (name.empty() || name.length() > 0x6F) {
			return false;
		}
		if (name.front() == '.' || (name.length() > 2 && name.substr(0, 2) == "..")) {
			return false;
		}
		if (isDirectory && (name.find('\\') != std::string::npos || name.find('/') != std::string::npos)) {
			return false;
		}
		// 检查文件名或目录名中的每个字符是否有效
		return std::all_of(name.begin(), name.end(), IsValidChar);
	}
} // namespace UnPdeC