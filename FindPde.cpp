#include "FindPde.h"
namespace UnpdeC {
	using std::vector;
	using std::string;
	using std::cout;
	using std::endl;
	using std::cin;

	/// <summary>
	/// 查找当前目录下所有的.pde文件
	/// </summary>
	/// <returns> .pde文件名列表 </returns>
	vector<PdeNames> FindPde::Get() {
		// Pde数组
		vector<PdeNames> PdeArr;
		string currentDirectory = std::filesystem::current_path().string();
		for (const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pde") {
				PdeNames NowPde;
				NowPde.FullName = entry.path().filename().string();
				NowPde.Name = NowPde.FullName.substr(0, NowPde.FullName.find_last_of("."));
				cout << " ！找到.pde文件：" << NowPde.FullName << endl;
				PdeArr.push_back(NowPde);
			}
		}

		// 如果没有找到.pde文件，则退出程序
		if (PdeArr.empty()) {
			cout << " ！目录下没有找到 *.pde文件！\n 按任意键退出程序";
			cin.get();
			exit(-1);
		}

		// 返回.pde文件名列表
		return PdeArr;
	}
}