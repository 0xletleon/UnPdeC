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
	/// 查找PDE文件
	/// </summary>
	class FindPde {
	public:
		/// <summary>
		/// 查找当前目录下所有的.pde文件
		/// </summary>
		/// <returns> .pde文件名列表 </returns>
		static vector<PdeNames> Get();
	};
}