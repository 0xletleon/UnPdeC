#include "UnPdeC.h"
#include "FindPde.h"
#include "PdeKey.h"

using std::cout;
using std::endl;
using std::vector;
using std::hex;
using namespace UnpdeC;

int main() {
	cout << "Hello CMake." << endl;
	vector<PdeNames> pdeList = UnpdeC::FindPde::Get();
	// 打印pdeList
	for (PdeNames pde : pdeList) {
		cout << pde.FullName << endl;
	}

	auto keyByte = UnpdeC::PdeKey::PDEKEY();
	// 打印keyByte
	for (int i = 0; i < keyByte.size(); i++) {
		cout << hex << (int)keyByte[i] << " ";
	}
	cout << endl;

	return 0;
}
