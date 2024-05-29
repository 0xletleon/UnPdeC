// PdeTool.h
#ifndef PDETOOL_H
#define PDETOOL_H

#include "Common.h"
#include "GV.h"
#include "PdeKey.h"

// 假设DataType和PdeNames等类型已经在适当的地方定义
namespace UnPdeC {

	/// <summary>
	/// PDE工具类
	/// </summary>
	class PdeTool {
		//private:
			//static uint32_t PDESIZE;
			//static std::vector<unsigned char> PDEKEY;
			//static PdeNames THISPDENAME;

	public:
		// 提供对静态成员的访问器函数，如果需要的话
		//static PdeNames ThisPdeName();

		// 初始化PDE工具类
		static void Init();

		//// 获取文件或文件夹偏移信息
		//static std::vector<HexOffsetInfo> GetOffsetInfo(const std::vector<unsigned char>& data, uint32_t BlockOffset);

		//// 从PDE文件中获取指定块数据
		//static GetOffsetStr GetByteOfPde(uint32_t Start, uint32_t Size);

		//// 解密文件或数据块
		//static std::vector<unsigned char> DeFileOrBlock(const std::vector<unsigned char>& OffsetArr);

		// 其他公共成员函数和变量...

		// 私有构造函数，以防止实例化
		//PdeTool() = delete;
	};

	// 在这里声明静态成员变量的定义（实现）将在.cpp文件中
   /* extern uint32_t PdeTool::PDESIZE;
	extern std::vector<unsigned char> PdeTool::PDEKEY;
	extern PdeNames PdeTool::THISPDENAME;*/

} // namespace Unpde

#endif // PDETOOL_H