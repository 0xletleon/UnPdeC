// FindPde.cpp
#include "FindPde.h"

namespace UnPdeC {

	vector<TNowPde> FindPde::Get() {
		// Pde文件名数组
		vector<TNowPde> PdeArr;

		// 获取可执行文件所在目录
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::filesystem::path exePath = std::filesystem::path(buffer).remove_filename();

		cout << " ！可执行文件所在目录：" << exePath.string() << endl;
		// 将程序目录写入全局变量
		GV::ExeDir = exePath;

		// 遍历可执行文件所在目录，查找所有 .pde 文件
		for (const auto& entry : std::filesystem::directory_iterator(exePath)) {
			if (entry.path().extension() == ".pde") {
				// 当前Pde信息
				TNowPde nowPde;

				// 从path对象获取文件名，并去除后缀名
				string fileName = entry.path().filename().string();
				fileName = fileName.substr(0, fileName.find_last_of('.'));
				nowPde.Name = fileName.substr(0, fileName.find_last_of('.'));

				// 获取当前文件的大小
				nowPde.Size = std::filesystem::file_size(entry.path());

				// 获取当前pde文件的路径
				nowPde.Path = entry.path().string();

				cout << " ！找到.pde文件：" << nowPde.Name << endl;
				cout << " ！文件大小：" << nowPde.Size << "字节" << endl;
				cout << " ！文件路径：" << nowPde.Path << endl;

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