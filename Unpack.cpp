// Unpack.cpp
#include "Unpack.h"

namespace UnPdeC {
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
				OffsetLog::AddEntry(BlockOffset, DirOrFile.OriginalOffset, DirOrFile.PdeOffset, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, Dir.NowDir.string());

				// 获取指定偏移的字节数据
				GetOffsetStr TempFileByte = PdeTool::GetByteOfPde(DirOrFile.PdeOffset, DirOrFile.Size);
				// 校验数据
				if (TempFileByte.Size != DirOrFile.Size) break;

				// 解密数据
				std::vector<unsigned char> DeTempFileByte = PdeTool::DeFileOrBlock(TempFileByte.Byte);

				/*		uint8_t temp_6f6d = DeTempFileByte[0x18];
						uint32_t tempsize;
						if (temp_6f6d == 0x6F) {
							tempsize = UnPdeC::UFunc::Get4Byte(DeTempFileByte, 0x19);
						} else if (temp_6f6d == 0x6D) {
							tempsize = DeTempFileByte[0x19];
						}

						std::cout << tempsize << std::endl;*/
						// 0x18 是头部大小，+0x4是为了修补解密时越界的问题！
						//uint32_t countsize = tempsize + 0x18 + 0x8;
						//std::cout << countsize << std::endl;

						//if (DirOrFile.Name == "skin_choujiang_bg01.tga.cache") {
						//	std::cout << " ！文件大小不正确: " << DirOrFile.Name << std::endl;
						//	//continue;
						//}

						//if (countsize == DeTempFileByte.size()) {
						//	std::cout << " ！文件大小正确: " << DirOrFile.Name << std::endl;
						//} else {
						//	std::cout << " ！文件大小不正确: " << DirOrFile.Name << std::endl;
						//}

						// 判断是否是空文件
				if (DeTempFileByte.empty() || DirOrFile.Name.empty()) break;

				// todo: 保存数据到DebugPde，调试时使用

				// 二次解密
				// 确保目录存在
				std::filesystem::path DirPath = GV::ExeDir / Dir.NowDir;
				if (!std::filesystem::exists(DirPath)) {
					std::filesystem::create_directories(DirPath);
				}

				if (DirOrFile.Name == "game_text.lua") {
					std::cout << " ！文件名是 game_text.lua" << std::endl;
				}

				// 检查文件名是否包含.cache来确定是否需要二次解密
				bool HasCache = DirOrFile.Name.find(".cache") != std::string::npos;

				// 跳过二次解密
				if (!HasCache) {
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