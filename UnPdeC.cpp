#include "UnPdeC.h"

int main() {
	cout << "Hello CMake." << endl;

	// 查找当前目录下所有的.pde文件
	vector<TNowPde> PdeArr = FindPde::Get();
	// 遍历PdeArr进行解密
	for (TNowPde pde : PdeArr) {
		cout << " ！正在解码：" << pde.Name << endl;
		// 设置当前解密的文件名
		GV::NowPde = pde;

		// 初始化PDETool
		PdeTool::Init();

		// 开始解密目录
		// 创建目录结构
		DirStr TryDir = { pde.Name + "/", pde.Name + "/" };
		Unpack::Try(0x1000, 0x1000, TryDir, false);
	}

	//auto keyByte = PdeKey::Get();
	//// 打印keyByte
	//for (int i = 0; i < keyByte.size(); i++) {
	//	cout << hex << (int)keyByte[i] << " ";
	//}
	//cout << endl;


	//cout << "GV::myGlobalVar = " << GV::myGlobalVar << endl;
	//GV::myGlobalVar = 98;
	//cout << "GV::myGlobalVar = " << dec << GV::myGlobalVar << endl;

	//PdeTool::Init();
	return 0;
}
