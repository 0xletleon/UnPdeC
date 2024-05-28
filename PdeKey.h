// PdeKey.h
#ifndef PDEKEY_H
#define PDEKEY_H

#include <iostream>
#include <vector>

namespace UnpdeC {
	using std::vector;

	class PdeKey {
	public:
		static vector<unsigned char> PDEKEY();
	};

} // namespace UnpdeC

#endif // PDEKEY_H