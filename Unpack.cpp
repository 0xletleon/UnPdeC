// Unpack.cpp
#include "Unpack.h"

namespace UnPdeC {
	// 暂时不参与二次解密的文件
	const vector<string> Unpack::PassArr = { ".fsb", ".swf", ".ttf", "version", "language" };

	bool Unpack::FindSuffix(const std::string& target) {
		// 将 DirOrFile.Name 转换成小写
		string NameLower = target;
		std::transform(NameLower.begin(), NameLower.end(), NameLower.begin(), ::tolower);

		// 查找
		return std::any_of(PassArr.begin(), PassArr.end(), [&NameLower](const std::string& str) {
			return NameLower.find(str) != std::string::npos;
			});
	}

	/// <summary>
	/// 尝试解密
	/// </summary>
	/// <param name="PdeOffset">数据块在PDE文件中的偏移值</param>
	/// <param name="Size">数据块大小</param>
	/// <param name="Dir">目录</param>
	/// <param name="Is170">是否为170表数据</param>
	void Unpack::Try(uint32_t PdeOffset, uint32_t Size, const DirStr& Dir, bool Is170) {
		std::cout << " ！正在尝试解密: " << Dir.NowDir << std::endl;

		//定义变量
		GetOffsetStr TryByte;
		vector<uint8_t> DeTryByte;

		try {
			// 读取数据
			TryByte = PdeTool::GetByteOfPde(PdeOffset, Size);
			// 校验数据
			//if (TryByte.Size != Size) return;
			// 解密数据块 -> 同时生成一个供调试时使用的PDE文件
			DeTryByte = PdeTool::DeFileOrBlock(TryByte.Byte);
		} catch (const std::exception& e) {
			std::cout << " ！读取数据失败: " << e.what() << std::endl;
			return;
		}

		// todo:保存数据到DebugPde，调试时使用

		// 非170表数据
		if (!Is170) {
			// 获取文件或文件夹偏移信息
			std::vector<HexOffsetInfo> DeTryByteArr = PdeTool::GetOffsetInfo(DeTryByte, PdeOffset);
			// 读取并保存文件到硬盘
			Save(DeTryByteArr, Dir, PdeOffset);
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
				//RecOffsetLog(BlockOffset, DirOrFile.PdeOffset, 0, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, Dir.NowDir);
				//GV::offsetLog.addEntry(6000, 7000, 0xD66, "idle.anim.cache", 1, "Root/animation/character/firstperson/boss/pose");
				OffsetLog::AddEntry(BlockOffset, DirOrFile.OriginalOffset, DirOrFile.PdeOffset, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, Dir.NowDir.string());
				//GV::NowPde.Name
				
				// 获取指定偏移的字节数据
				GetOffsetStr TempFileByte = PdeTool::GetByteOfPde(DirOrFile.PdeOffset, DirOrFile.Size);
				// 校验数据
				if (TempFileByte.Size != DirOrFile.Size) break;

				// 解密数据
				std::vector<unsigned char> DeTempFileByte = PdeTool::DeFileOrBlock(TempFileByte.Byte);

				// 判断是否是空文件
				if (DeTempFileByte.empty() || DirOrFile.Name.empty()) break;

				// todo: 保存数据到DebugPde，调试时使用

				// 二次解密

				// 跳过二次解密
				if (FindSuffix(DirOrFile.Name)) {
					// 保存文件
					std::filesystem::path FilePath = GV::ExeDir / Dir.NowDir / DirOrFile.Name;
					// 判断文件是否存在
					if (!std::filesystem::exists(FilePath)) {
						// 保存文件
						try {
							std::ofstream outFile(FilePath, std::ios::binary);
							outFile.write(reinterpret_cast<const char*>(DeTempFileByte.data()), DeTempFileByte.size());
							outFile.close();
						} catch (const std::exception&) {
							cout << " ！保存文件失败: " << DirOrFile.Name << endl;
						}
					}
				} else {
					// 二次解密
					std::vector<unsigned char> Decryption2;
					std::string fixName = DirOrFile.Name;
					try {
						Decryption2 = FinalUnpack::PreDecrypt(DeTempFileByte, DirOrFile.Name);
						// 去除 DirOrFile.Name 中的 .cache
						fixName = DirOrFile.Name.substr(0, DirOrFile.Name.find(".cache"));
					} catch (const std::exception&) {
						cout << " ！二次解密失败: " << DirOrFile.Name << endl;
						Decryption2 = DeTempFileByte;
					}

					// 保存文件
					std::filesystem::path FilePath2 = GV::ExeDir / Dir.NowDir / fixName;
					// 判断文件是否存在
					if (!std::filesystem::exists(FilePath2)) {
						// 保存文件
						try {
							std::ofstream outFile(FilePath2, std::ios::binary);
							outFile.write(reinterpret_cast<const char*>(Decryption2.data()), Decryption2.size());
							outFile.close();
						} catch (const std::exception&) {
							cout << " ！保存文件失败: " << DirOrFile.Name << endl;
						}
					}
				}
			} else if (DirOrFile.Type == 2) { // 目录
				// todo: 记录目录偏移信息
				//RecOffsetLog(blockOffset, DirOrFile.PdeOffset, 0, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, dir.NowDir);
				OffsetLog::AddEntry(BlockOffset, DirOrFile.OriginalOffset, DirOrFile.PdeOffset, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, Dir.NowDir.string());

				// 拼接新目录路径
				DirStr NewDir = { Dir.NowDir, Dir.NowDir / DirOrFile.Name };

				// 创建目录
				try {
					std::filesystem::path DirPath = GV::ExeDir / NewDir.NowDir;
					if (!std::filesystem::exists(DirPath)) {
						std::filesystem::create_directory(DirPath);
						//cout << " ！创建目录成功!!!!: " << DirPath << endl;
					}

				} catch (const std::filesystem::filesystem_error& e) {
					cerr << "创建目录时发生错误: " << e.what() << endl;
				}

				// 递归解密
				Unpack::Try(DirOrFile.PdeOffset, DirOrFile.Size, NewDir, false);
			} else {
				std::cout << "其他类型: " << DirOrFile.Name << std::endl;
			}
		}
	}
}