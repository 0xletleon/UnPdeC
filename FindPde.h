// FindPde.h
#ifndef FINDPDE_H
#define FINDPDE_H

#include "Common.h"

namespace UnPdeC {

	///// <summary>
	///// ��ǰPde�ļ���Ϣ
	///// </summary>
	//struct TNowPde {
	//	string Name;
	//	long Size;
	//};

	/// <summary>
	/// ����PDE�ļ�
	/// </summary>
	class FindPde {
	public:
		/// <summary>
		/// ���ҵ�ǰĿ¼�����е�.pde�ļ�
		/// </summary>
		/// <returns> .pde�ļ����б� </returns>
		static vector<TNowPde> Get();
	};
}

#endif // FINDPDE_H