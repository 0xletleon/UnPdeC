#include "FindPde.h"

namespace UnPdeC {

	vector<TNowPde> FindPde::Get() {
		// Pde�ļ�������
		vector<TNowPde> PdeArr;
		string currentDirectory = std::filesystem::current_path().string();
		for (const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pde") {
				TNowPde nowPde;
				// ��path�����ȡ�ļ�������ȥ����׺��
				string fileName = entry.path().filename().string();
				fileName = fileName.substr(0, fileName.find_last_of('.'));
				nowPde.Name = fileName.substr(0, fileName.find_last_of('.'));

				// ��ȡ��ǰ�ļ��Ĵ�С
				nowPde.Size = std::filesystem::file_size(entry.path());

				// ��ȡ��ǰpde�ļ���·��
				nowPde.Path = entry.path().string();

				cout << " ���ҵ�.pde�ļ���" << nowPde.Name << endl;
				cout << " ���ļ���С��" << nowPde.Size << "�ֽ�" << endl;
				cout << " ���ļ�·����" << nowPde.Path << endl;

				PdeArr.push_back(nowPde);
			}
		}

		// ���û���ҵ�.pde�ļ������˳�����
		if (PdeArr.empty()) {
			cout << " ��Ŀ¼��û���ҵ� *.pde�ļ���\n ��������˳�����";
			cin.get();
			exit(-1);
		}

		// ����.pde�ļ����б�
		return PdeArr;
	}
}