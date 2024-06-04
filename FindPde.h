// FindPde.h
#ifndef FINDPDE_H
#define FINDPDE_H

#include "Common.h"
#include <windows.h>

namespace UnPdeC {

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