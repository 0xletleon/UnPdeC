// Unpack.h
#ifndef UNPACK_H
#define UNPACK_H

#include "Common.h"
#include "PdeTool.h"

//#include <vector>
//#include <string>
//#include <iostream>
//#include <filesystem>
//#include <optional>

namespace UnPdeC {
	class Unpack {
	private:
		// ��ʱ��������ν��ܵ��ļ�
		static const vector<string> PassArr;

	public:
		/// <summary>
		/// ���Խ���
		/// </summary>
		/// <param name="Offset">���ݿ���PDE�ļ��е�ƫ��ֵ</param>
		/// <param name="Size">���ݿ��С</param>
		/// <param name="Dir">Ŀ¼</param>
		/// <param name="Is170">�Ƿ�Ϊ170������</param>
		static void Try(uint32_t Offset, uint32_t Size, const DirStr& Dir, bool Is170);

		/// <summary>
		/// ����Ŀ¼���ļ�
		/// </summary>
		/// <param name="DirOrFileArr">�ļ���Ŀ¼����</param>
		/// <param name="Dir">Ŀ¼</param>
		/// <param name="BlockOffset">���ݿ���PDE�ļ��е�ƫ��ֵ</param>
		static void Save(const vector<HexOffsetInfo>& DirOrFileArr, const DirStr& Dir, uint32_t BlockOffset);
	};

} // namespace Unpde

#endif // UNPACK_H