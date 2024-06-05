#include "UnPdeC.h"

int main() {
	cout << "Hello CMake." << endl;

	//try {
	//	//文件名 155
	//	string finaName = "loadingmap_level_train.dds.cache";
	//	// 获取程序所在目录并拼接文件名
	//	std::filesystem::path file_path = std::filesystem::current_path();

	//	// 打开文件
	//	std::ifstream file(file_path / "TestFile" / finaName, std::ios::binary);
	//	if (!file) {
	//		std::cerr << "无法打开文件: " << file_path / "TestFile" / finaName << std::endl;
	//		return 1;
	//	}

	//	// 读取文件内容到字节向量中
	//	std::vector<uint8_t> encrypted_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	//	// 关闭文件
	//	file.close();

	//	std::vector<uint8_t> xaa = FinalUnpack::PreDecrypt(encrypted_data, finaName);
	//	cout << "xaa size: " << xaa.size() << endl;
	//	// 将 xaa 写入程序所在目录
	//	std::ofstream out("TestFile/" + finaName + ".bin", std::ios::binary);
	//	out.write((char*)&xaa[0], xaa.size());
	//	out.close();
	//} catch (const std::filesystem::filesystem_error& e) {
	//	std::cerr << "文件系统错误: " << e.what() << std::endl;
	//	return 1;
	//} catch (const std::exception& e) {
	//	std::cerr << "异常: " << e.what() << std::endl;
	//	return 1;
	//}

	// 查找当前目录下所有的.pde文件
	vector<TNowPde> PdeArr = FindPde::Get();
	//遍历PdeArr进行解密
	for (TNowPde pde : PdeArr) {
		cout << " ！正在解码：" << pde.Name << endl;
		// 设置当前解密的文件名
		GV::NowPde = pde;

		// 初始化PDETool
		PdeTool::Init();

		// 创建目录结构
		DirStr TryDir = { pde.Name , pde.Name };
		// 开始解密目录
		Unpack::Try(0x1000, 0x1000, TryDir, false);
	}

	//cout << "解密完成！" << endl;
	return 0;
}
