#include "FindPde.h"

namespace UnPdeC {

	std::vector<TNowPde> FindPde::Get() {
		// Pde文件名数组
		std::vector<TNowPde> PdeArr;

		// 获取可执行文件所在目录
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::filesystem::path exePath = std::filesystem::path(buffer).remove_filename();

		std::cout << " ！可执行文件所在目录：" << exePath.string() << "\n";

		GV::ExeDir = exePath;// 将程序目录写入全局变量

		// 遍历可执行文件所在目录，查找所有 .pde 文件
		for (const auto& entry : std::filesystem::directory_iterator(exePath)) {
			if (entry.path().extension() == ".xor") {
				// 当前Pde信息
				TNowPde nowPde;

				// 从path对象获取文件名，并去除后缀名
				std::string fileName = entry.path().filename().string();
				fileName = fileName.substr(0, fileName.find_first_of('.'));
				nowPde.Name = fileName;

				nowPde.Size = std::filesystem::file_size(entry.path());// 获取当前文件的大小

				nowPde.Path = entry.path().string();// 获取当前pde文件的路径

				std::cout << " ！找到.pde文件：" << nowPde.Name << "\n";
				std::cout << " ！文件大小：" << nowPde.Size << "字节\n";
				std::cout << " ！文件路径：" << nowPde.Path << "\n";

				PdeArr.push_back(nowPde);
			}
		}

		// 如果没有找到.pde文件，则退出程序
		if (PdeArr.empty()) {
			std::cout << " ！目录下没有找到 *.pde文件！\n 按任意键退出程序\n";
			std::cin.get();
			exit(-1);
		}

		// 返回.pde文件名列表
		return PdeArr;
	}
}