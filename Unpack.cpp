#include "Unpack.h"

namespace UnPdeC {
	// 暂时不参与二次解密的文件
	const vector<string> PassArr = { ".fsb", ".swf", ".ttf", "version", "language" };

	/// <summary>
	/// 尝试解密
	/// </summary>
	/// <param name="Offset">数据块在PDE文件中的偏移值</param>
	/// <param name="Size">数据块大小</param>
	/// <param name="Dir">目录</param>
	/// <param name="Is170">是否为170表数据</param>
	void Unpack::Try(uint32_t Offset, uint32_t Size, const DirStr& Dir, bool Is170) {
		std::cout << " ！正在尝试解密: " << Dir.NowDir << std::endl;

		//定义变量
		GetOffsetStr TryByte;
		vector<uint8_t> DeTryByte;

		try {
			// 读取数据
			TryByte = PdeTool::GetByteOfPde(Offset, Size);
			// 校验数据
			//if (TryByte.Size != Size) return;
			// 解密数据块 -> 同时生成一个供调试时使用的PDE文件
			DeTryByte = PdeTool::DeFileOrBlock(TryByte.Byte);

			// 将 DeTryByte 保存到当前文件夹下
			//string FileName = "Testx.hex";
			//std::ofstream ofs(FileName, std::ios::binary);
			//ofs.write(reinterpret_cast<const char*>(DeTryByte.data()), DeTryByte.size());
			//ofs.close();

			//std::cout << " ！解密成功: " << FileName << std::endl;


		} catch (const std::exception& e) {
			std::cout << " ！读取数据失败: " << e.what() << std::endl;
			return;
		}

		// 非170表数据
		if (!Is170) {
			// 获取文件或文件夹偏移信息
			std::vector<HexOffsetInfo> DeTryByteArr = PdeTool::GetOffsetInfo(DeTryByte, Offset);
			// 读取并保存文件到硬盘
			Save(DeTryByteArr, Dir, Offset);
		}

	}

	/// <summary>
	/// 创建目录或文件
	/// </summary>
	/// <param name="DirOrFileArr">文件或目录数组</param>
	/// <param name="Dir">目录</param>
	/// <param name="BlockOffset">数据块在PDE文件中的偏移值</param>
	void Unpack::Save(const vector<HexOffsetInfo>& DirOrFileArr, const DirStr& Dir, uint32_t BlockOffset) {

		for (const HexOffsetInfo DirOrFile : DirOrFileArr) {
			if (DirOrFile.Type == 1) { // 文件
				// 记录文件偏移信息
				//RecOffsetLog(blockOffset, DirOrFile.Offset, 0, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, dir.NowDir);

				// 获取指定偏移的字节数据
				GetOffsetStr TempFileByte = PdeTool::GetByteOfPde(DirOrFile.Offset, DirOrFile.Size);
				// 校验数据
				if (TempFileByte.Size != DirOrFile.Size) break;

				// 解密数据
				std::vector<unsigned char> DeTempFileByte = PdeTool::DeFileOrBlock(TempFileByte.Byte);

				// 判断是否是空文件
				if (DeTempFileByte.empty() || DirOrFile.Name.empty()) break;

				// 保存文件
				std::string savePath = Dir.NowDir + DirOrFile.Name;
				if (!std::filesystem::exists(savePath)) {
					// 保存文件
					std::ofstream outFile(savePath, std::ios::binary);
					outFile.write(reinterpret_cast<const char*>(DeTempFileByte.data()), DeTempFileByte.size());
					outFile.close();
				}
			} else if (DirOrFile.Type == 2) { // 目录
				// 记录目录偏移信息
				//RecOffsetLog(blockOffset, DirOrFile.Offset, 0, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, dir.NowDir);

				// 创建目录
				std::filesystem::create_directory(dir.NowDir + DirOrFile.Name + "/");
				// 递归解密
				// Try(DirOrFile.Offset, DirOrFile.Size, new DirStr(...));
			} else {
				std::cout << "其他类型: " << DirOrFile.Name << std::endl;
			}
		}

	}

}