#include "PdeTool.h"

namespace UnPdeC {
	void PdeTool::Init() {
		cout << "���ڳ�ʼ�� PdeTool" << endl;
	
        // ���Ŀ¼�Ƿ����
        if (!std::filesystem::exists(GV::NowPde.Name)) {
            // Ŀ¼�����ڣ�������
            std::filesystem::create_directory(GV::NowPde.Name);
            std::cout << "�Ѵ���Ŀ¼: " << GV::NowPde.Name << std::endl;
        }

        // ��ȡPdeKey
        PdeKey::Get();

        cout << " �� �ɹ���ʼ��PDE������" << endl;
	}

}