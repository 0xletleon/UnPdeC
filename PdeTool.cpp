// PdeTool.cpp
#include "PdeTool.h"

namespace UnPdeC {
	void PdeTool::Init() {
		cout << "正在初始化 PdeTool" << endl;

		// 检查目录是否存在
		std::filesystem::path PdeDir = GV::ExeDir / GV::NowPde.Name;
		if (!std::filesystem::exists(PdeDir)) {
			// 目录不存在，创建它
			try {
				std::filesystem::create_directory(PdeDir);
				std::cout << "已创建目录: " << PdeDir << std::endl;
			} catch (const std::exception& e) {
				std::cerr << "创建目录失败: " << e.what() << std::endl;
				return;
			}
		}

		// 获取PdeKey
		PdeKey::Get();

		cout << " √ 成功初始化PDE工具类" << endl;
	}

	/// <summary>
	/// 获取文件或文件夹偏移信息
	/// </summary>
	/// <param name="data">初次解密后的数据</param>
	/// <param name="BlockOffset">数据块在PDE文件中的偏移值</param>
	/// <returns>文件或文件夹的偏移信息数组 </returns>
	std::vector<HexOffsetInfo> PdeTool::GetOffsetInfo(const std::vector<unsigned char>& data, uint32_t BlockOffset) {
		const int BlockSize = 128;
		size_t BlockCount = data.size() / BlockSize;
		std::vector<std::vector<char>> BlockArr(BlockCount);

		// 循环分块
		for (int i = 0; i < BlockCount; ++i) {
			int start = i * BlockSize;
			int length = std::min(BlockSize, static_cast<int>(data.size()) - start);
			BlockArr[i].resize(length);
			std::copy(data.begin() + start, data.begin() + start + length, BlockArr[i].begin());
		}

		// 返回的偏移信息数组
		std::vector<HexOffsetInfo> OffsetInfoArr;

		// 循环获取偏移,大小,类型,文件名
		for (int bi = 0; bi < BlockCount; ++bi) {
			const std::vector<char>& Block = BlockArr[bi];
			HexOffsetInfo ThisInfo{ 0, "", 0, 0, 0 };

			// 读取类型 1==文件，2==目录
			int NonZeroIndex = 0;
			for (int i = 0; i < Block.size(); ++i) {
				if (i == 0) {
					ThisInfo.Type = Block[i];
					// 验证类型
					if (ThisInfo.Type != 1 && ThisInfo.Type != 2) break;
				} else {
					// 获取名称
					int sub = Block[i] - 0x41;
					if (sub <= 25) sub += 32;
					if (sub <= 0) {
						NonZeroIndex = i - 1;
						break;
					}
				}
			}

			if (NonZeroIndex > 0) {
				// 复制名称
				ThisInfo.Name = std::string(Block.begin() + 1, Block.begin() + NonZeroIndex + 1);
				if (!NameValidator::Check(ThisInfo.Type, ThisInfo.Name)) {
					break;
				}
			} else {
				// 获取文件名出错
				break;
			}

			// 读取大小
			// 使用C++17的构造器直接初始化最后4个字节
			std::vector<uint8_t> SizeBytes(Block.end() - 4, Block.end());
			union {
				uint32_t value;
				uint8_t bytes[4];
			} ThisSize{};
			// 直接将字节赋值给data的bytes数组，这会利用大端字节序
			std::copy(SizeBytes.begin(), SizeBytes.end(), ThisSize.bytes);

			// 现在data.value包含了合并后的32位数值
			std::cout << "0x" << std::hex << ThisSize.value << std::endl;
			ThisInfo.Size = ThisSize.value;
			cout << "文件名: " << ThisInfo.Name << " 大小: " << ThisInfo.Size << " 类型: " << (ThisInfo.Type == 1 ? "文件" : "目录") << endl;


			// 读取原始偏移值
			// 使用C++17的构造器直接初始化最后4个字节
			std::vector<uint8_t> OriginalOffsetBytes(Block.end() - 8, Block.end() - 4);
			union {
				uint32_t value;
				uint8_t bytes[4];
			} OriginalOffset{};
			// 直接将字节赋值给data的bytes数组，这会利用大端字节序
			std::copy(OriginalOffsetBytes.begin(), OriginalOffsetBytes.end(), OriginalOffset.bytes);

			// 现在data.value包含了合并后的32位数值
			std::cout << "0x" << std::hex << OriginalOffset.value << std::endl;
			ThisInfo.OriginalOffset = OriginalOffset.value;


			// 计算实际偏移值
			ThisInfo.PdeOffset = ((ThisInfo.OriginalOffset >> 10) + ThisInfo.OriginalOffset + 1) << 12;
			cout << "PdeOffset: " << ThisInfo.PdeOffset << endl;

			// 如果 ThisInfo.PdeOffset 越界，则退出循环
			if (ThisInfo.PdeOffset >= GV::NowPde.Size) {
				std::cerr << "ThisInfo.PdeOffset 越界退出循环" << std::endl;
				continue;
			}

			// todo: 读取标识
			//// 读取标识
			//std::vector<uint8_t> TagBytes(Block.end() - 0x10, Block.end() - 0xc);
			//uint32_t TagOffSet = BlockOffset + ((bi + 1) * 0x80) - 0xC;
			//GetOffsetStr ThisTag = GetByteOfPde(TagOffSet, 4);
			//// 打印 ThisTag.Byte

			//// 添加到OffsetInfoArr
			OffsetInfoArr.push_back(ThisInfo);
		}

		return OffsetInfoArr;
	}

	/// <summary>
	/// 从PDE文件中获取指定块数据
	/// </summary>
	/// <param name="Start">块在PDE文件中的起始偏移</param>
	/// <param name="Size">块大小</param>
	/// <returns>块数据</returns>
	GetOffsetStr PdeTool::GetByteOfPde(uint32_t Start, uint32_t Size) {
		// 打开文件用于读取二进制数据
		ifstream PDEFILE_FS(GV::NowPde.Path, ios::binary);

		// 检查文件是否打开成功
		if (!PDEFILE_FS) {
			cerr << "无法打开PDE文件: " << GV::NowPde.Name << endl;
			// 退出程序
			exit(1);
		}

		// 设置读取起点
		PDEFILE_FS.seekg(Start, ios::beg);

		// 检查文件位置是否正确
		if (PDEFILE_FS.tellg() != Start) {
			cerr << "文件定位失败" << endl;
			return { 0, vector<uint8_t>() };
		}

		vector<uint8_t> buffer(Size);
		PDEFILE_FS.read(reinterpret_cast<char*>(buffer.data()), Size);

		if (!PDEFILE_FS) {
			cerr << "读取PDE文件失败" << endl;
			// 如果读取失败，返回空的GetOffsetStr
			return { 0, vector<uint8_t>() };
		}

		return { static_cast<uint32_t>(PDEFILE_FS.gcount()), buffer };
	}

	/// <summary>
	/// 解密文件 或! 数据块
	/// </summary>
	/// <param name="OffsetArr">文件或数据块的偏移信息数组</param>
	/// <returns>解密后的数据</returns>
	std::vector<unsigned char> PdeTool::DeFileOrBlock(const std::vector<unsigned char>& OffsetArr) {
		// 当前临时解密字节数组
		std::vector<uint8_t> TempDEArr(OffsetArr.size(), 0);

		// Key长度
		//size_t KeyLength = GV::PdeKey.size() - 1;
		// GV::PdeKey 的固定长度
		constexpr size_t KeyLength = 0x1000;

		// 遍历 OffsetArr 并使用 GV::PdeKey 进行解密
		for (size_t i = 0; i < OffsetArr.size(); ++i) {
			// 使用模运算确保 KeyIndex 循环在 0 到 KeyLength 之间
			size_t KeyIndex = i % KeyLength;
			TempDEArr[i] = GV::PdeKey[KeyIndex] ^ OffsetArr[i];
		}

		// 完成解密数据块
		return TempDEArr;
	}
}