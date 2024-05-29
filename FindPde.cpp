#include "FindPde.h"

namespace UnPdeC {

	vector<TNowPde> FindPde::Get() {
		// Pde文件名数组
		vector<TNowPde> PdeArr;
		string currentDirectory = std::filesystem::current_path().string();
		for (const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pde") {
				TNowPde nowPde;
				// 从path对象获取文件名，并去除后缀名
				string fileName = entry.path().filename().string();
				fileName = fileName.substr(0, fileName.find_last_of('.'));
				nowPde.Name = fileName.substr(0, fileName.find_last_of('.'));

				// 获取当前文件的大小
				nowPde.Size = std::filesystem::file_size(entry.path());

				cout << " ！找到.pde文件：" << nowPde.Name << endl;
				cout << " ！文件大小：" << nowPde.Size << "字节" << endl;

				PdeArr.push_back(nowPde);
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