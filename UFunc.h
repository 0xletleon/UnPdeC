// UFunc.h
#ifndef UFUNC_H
#define UFUNC_H

#include "Common.h"

namespace UnPdeC {
	/// <summary>
	/// ʵ�÷�����
	/// </summary>
	class UFunc {
	public:
		/// <summary>
		///  ��ȡ4�ֽ�����
		/// </summary>
		/// <param name="data"> ����</param>
		/// <param name="offset"> ƫ��</param>
		/// <returns> 4�ֽ�����</returns>
		static std::uint32_t Get4Byte(const std::vector<std::uint8_t>& data, std::size_t offset);
	};
}
#endif // UFUNC_H
