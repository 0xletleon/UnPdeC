#include "PdeTool.h"

namespace UnPdeC {
	void PdeTool::Init() {
		cout << "正在初始化 PdeTool" << endl;
	
        // 检查目录是否存在
        if (!std::filesystem::exists(GV::NowPde.Name)) {
            // 目录不存在，创建它
            std::filesystem::create_directory(GV::NowPde.Name);
            std::cout << "已创建目录: " << GV::NowPde.Name << std::endl;
        }

        // 获取PdeKey
        PdeKey::Get();

        cout << " √ 成功初始化PDE工具类" << endl;
	}

}