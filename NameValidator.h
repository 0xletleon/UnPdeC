// NameValidator.h
#ifndef NAMEVALIDATOR_H
#define NAMEVALIDATOR_H

#include "Common.h"

namespace UnPdeC {

	class NameValidator {
	public:
		/// <summary>
		/// 检查
		/// </summary>
		/// <param name="Type">类型</param>
		/// <param name="Name">名称</param>
		/// <returns>是否合法</returns>
		static bool Check(uint8_t Type, const std::string& Name);

	private:
		/// <summary>
		///  检查字符是否合法
		/// </summary>
		/// <param name="c"> 字符 </param>
		/// <returns> 是否合法 </returns>
		static bool IsValidChar(char c);
		/// <summary>
		/// 检查文件/目录名是否合法
		/// </summary>
		/// <param name="name"> 文件名或目录名 </param>
		/// <param name="isDirectory"> 是否为目录 </param>
		/// <returns></returns>
		static bool IsValidName(const std::string& name, bool isDirectory);
	};

} // namespace UnPdeC

#endif // NAMEVALIDATOR_H