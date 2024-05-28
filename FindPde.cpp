#include "FindPde.h"
namespace UnpdeC {
	using std::vector;
	using std::string;
	using std::cout;
	using std::endl;
	using std::cin;

	/// <summary>
	/// ���ҵ�ǰĿ¼�����е�.pde�ļ�
	/// </summary>
	/// <returns> .pde�ļ����б� </returns>
	vector<PdeNames> FindPde::Get() {
		// Pde����
		vector<PdeNames> PdeArr;
		string currentDirectory = std::filesystem::current_path().string();
		for (const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pde") {
				PdeNames NowPde;
				NowPde.FullName = entry.path().filename().string();
				NowPde.Name = NowPde.FullName.substr(0, NowPde.FullName.find_last_of("."));
				cout << " ���ҵ�.pde�ļ���" << NowPde.FullName << endl;
				PdeArr.push_back(NowPde);
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