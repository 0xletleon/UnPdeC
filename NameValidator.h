// NameValidator.h
#ifndef NAMEVALIDATOR_H
#define NAMEVALIDATOR_H

#include "Common.h"
#include <string>
#include <regex>

namespace UnPdeC {

	class NameValidator {
	public:
		/// <summary>
		/// ���
		/// </summary>
		/// <param name="Type">����</param>
		/// <param name="Name">����</param>
		/// <returns>�Ƿ�Ϸ�</returns>
		static bool Check(uint8_t Type, const std::string& Name);

	private:
		/// <summary>
		///  ����ַ��Ƿ�Ϸ�
		/// </summary>
		/// <param name="c"> �ַ� </param>
		/// <returns> �Ƿ�Ϸ� </returns>
		static bool IsValidChar(char c);
		/// <summary>
		/// ����ļ�/Ŀ¼���Ƿ�Ϸ�
		/// </summary>
		/// <param name="name"> �ļ�����Ŀ¼�� </param>
		/// <param name="isDirectory"> �Ƿ�ΪĿ¼ </param>
		/// <returns></returns>
		static bool IsValidName(const std::string& name, bool isDirectory);
	};

} // namespace Unpde

#endif // NAMEVALIDATOR_H