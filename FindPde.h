// FindPde.h
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>

namespace UnpdeC {
	using std::string;
	using std::vector;

	/// <summary>
	/// PdeNames struct
	/// </summary>
	struct PdeNames {
		string Name;
		string FullName;
	};

	/// <summary>
	/// ����PDE�ļ�
	/// </summary>
	class FindPde {
	public:
		/// <summary>
		/// ���ҵ�ǰĿ¼�����е�.pde�ļ�
		/// </summary>
		/// <returns> .pde�ļ����б� </returns>
		static vector<PdeNames> Get();
	};
}