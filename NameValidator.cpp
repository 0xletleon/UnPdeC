// NameValidator.cpp
#include "NameValidator.h"

namespace UnPdeC {
	bool NameValidator::Check(uint8_t Type, const string& Name) {
		if (Type == 1) {
			// �ж��ļ����Ƿ�Ϸ�
			return IsValidName(Name, false);
		} else if (Type == 2) {
			// �ж�Ŀ¼���Ƿ�Ϸ�
			return IsValidName(Name, true);
		}
		return false;
	}

	/// <summary>
	///  ����ַ��Ƿ�Ϸ�
	/// </summary>
	/// <param name="c"> �ַ� </param>
	/// <returns> �Ƿ�Ϸ� </returns>
	bool NameValidator::IsValidChar(char c) {
		return c == '_' || c == '-' || c == '\\' || c == '"' || c == '.' ||
			std::isdigit(c) || std::isalpha(c);
	}

	/// <summary>
	/// ����ļ�/Ŀ¼���Ƿ�Ϸ�
	/// </summary>
	/// <param name="name"> �ļ�����Ŀ¼�� </param>
	/// <param name="isDirectory"> �Ƿ�ΪĿ¼ </param>
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
		// ����ļ�����Ŀ¼���е�ÿ���ַ��Ƿ���Ч
		return std::all_of(name.begin(), name.end(), IsValidChar);
	}
} // namespace UnPdeC