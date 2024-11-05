#include "Mata.h"

namespace UnPdeC {
	/// <summary>
	/// 提取元信息
	/// </summary>
	/// <param name="MataBlock">块数据</param>
	/// <param name="XorFileSize">.xor文件大小</param>
	/// <returns>元信息</returns>
	BlockInfo Mata::ExtractMataInfo(const uint8_t* MataBlock, uint64_t XorFileSize) {
		// 提取到的文件信息
		BlockInfo blockInfo{};

		// 判断文件名是否合法
		if (MataBlock[0] != 0x01 && MataBlock[0] != 0x02) {
			return blockInfo;
		}

		// 判断文件名是否合法
		std::string name{ &MataBlock[1], std::find(&MataBlock[1], &MataBlock[0x50], '\0') };
		if (!Tools::NameCheck(name)) {
			return blockInfo;
		}

		// 判断此区域是否位0x0
		if (!std::all_of(MataBlock + 0x50, MataBlock + 0x70, [](uint8_t c) { return c == 0x00; })) {
			return blockInfo;
		}

		// 获取并判断编码后的偏移值是否合法
		uint64_t originalOffset = *reinterpret_cast<const uint32_t*>(&MataBlock[0x78]);
		if (originalOffset < 1) {
			return blockInfo;
		}

		// 读取并判断文件大小是否合法
		uint64_t size = *reinterpret_cast<const uint32_t*>(&MataBlock[0x7c]);
		if (size >= XorFileSize || size == 0) {
			return blockInfo;
		}

		// 从.xor中读取文件信息
		struct BlockInfo {
			bool isValid = false; // 块是否有效
			std::string type; // "1" for file, "2" for folder
			std::string name; // 文件名
			std::string encodedOffset; // 编码后的偏移值
			std::string decodedOffset; // 解码后的偏移值
			std::string size; // 数据块大小
			std::string blockOffset; // 块的偏移值
			std::string blockEncodedOffset; // 块的编码后的偏移值
		};

		// 拼接信息
		blockInfo = {
			true,
			(MataBlock[0] == 0x01) ? "1" : "2",
			name,
			Tools::ToHexString(originalOffset),
			Tools::ToHexString(Tools::OrigToPdeOffset(originalOffset)),
			Tools::ToHexString(size),
			"",
			""
		};

		// 返回元信息
		return blockInfo;
	}

	/// <summary>
	/// 读取.xor中所有元数据块
	/// </summary>
	/// <param name="FilePath">.xor文件路径</param>
	/// <returns>所有文件/夹的元数据json</returns>
	nlohmann::json Mata::ReadMatakForXorFile(const std::filesystem::path& FilePath) {
		std::cout << "\n ！Reading Mate information......\n";

		if (!std::filesystem::exists(FilePath)) {
			throw std::runtime_error("File does not exist: " + FilePath.string());
		}

		// 读取文件
		std::ifstream file(FilePath, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Unable to open file: " + FilePath.string());
		}

		file.seekg(0, std::ios::end);// 获取文件大小
		uint64_t fileSize = file.tellg();// .xor文件大小
		file.seekg(0x1000, std::ios::beg);// 跳过文件头 0x1000是根目录

		const uint64_t blockSize = 0x80;// 块大小
		std::vector<uint8_t> buffer(blockSize);// 缓冲区

		nlohmann::json maps;// 存储目录与文件映射信息
		nlohmann::json list;// 存储目录与文件元信息

		uint64_t offsetIndex = 0x1000;// 跳过的偏移量

		while (file.read(reinterpret_cast<char*>(buffer.data()), blockSize)) {
			// 读取并验证块
			BlockInfo BlockInfo = Mata::ExtractMataInfo(buffer.data(), fileSize);

			if (BlockInfo.isValid) {
				// 获取当前偏移值对应的编码偏移值
				auto blockEncodedOffset = Tools::ToHexString(Tools::PdeToOrigOffset(offsetIndex));
				auto blockOffsetStr = Tools::ToHexString(offsetIndex);
				// 存储目录与文件映射信息
				maps[Tools::ToHexString(Tools::PdeToOrigOffset(offsetIndex))].push_back(BlockInfo.encodedOffset);
				// 存储目录与文件元信息
				list[BlockInfo.encodedOffset] = {
					{"type", BlockInfo.type},
					{"name", BlockInfo.name},
					{"encodedOffset", BlockInfo.encodedOffset},
					{"decodedOffset", BlockInfo.decodedOffset},
					{"size", BlockInfo.size},
					{"blockOffset", blockOffsetStr},
					{"blockEncodedOffset", blockEncodedOffset}
				};
			}
			// 下一个块
			offsetIndex += blockSize;
		}

		// 合并JSON
		nlohmann::json output = {
			{"maps", maps},
			{"list", list}
		};

		// 保存元信息到磁盘
		std::string outputPath = (GV::ExeDir / (GV::NowXor.Name + ".json")).string();
		std::ofstream outFile(outputPath);
		outFile << output.dump(4);
		outFile.close();

		std::cout << " << JSON: " << outputPath << "\n √ Meta information reading completed\n";

		// 返回JSON
		return output;
	}

	/// <summary>
	/// 解码并保存文件/夹
	/// </summary>
	/// <param name="MataJson">待保文件夹元信息</param>
	void Mata::DecodeAndSaveFile(const MataSaveInfo& MataJson) {
		// 文件
		if (MataJson.type == "1") {
			// 读取文件
			GetOffsetStr tempFileByte = Tools::GetByteOfXor(MataJson.offset, MataJson.size);
			// 检查文件大小
			if (tempFileByte.Size != MataJson.size) return;

			// Ensure directory exists
			std::filesystem::path dirPath = MataJson.directoryPath;
			if (!std::filesystem::exists(dirPath)) {
				std::filesystem::create_directories(dirPath);
			}

			// Check if the file name contains ".cache" to determine if it needs secondary decryption
			bool hasCache = MataJson.name.find(".cache") != std::string::npos;
			std::string fixedName;

			// 无需二次解码
			if (!hasCache) {
				// 检查 fixedName 是否以 .lua 结尾
				if (MataJson.name.size() >= 4 && MataJson.name.substr(MataJson.name.size() - 4) == ".lua") {
					// 更改后缀为 .luac
					fixedName = MataJson.name.substr(0, MataJson.name.size() - 4) + ".luac";
				} else {
					// 赋值为原始的名字
					fixedName = MataJson.name;
				}

				// Save file
				std::filesystem::path filePath = dirPath / fixedName;
				if (!std::filesystem::exists(filePath)) {
					std::ofstream outFile(filePath, std::ios::binary);
					outFile.write(reinterpret_cast<const char*>(tempFileByte.Byte.data()), tempFileByte.Byte.size());
					outFile.close();
				}
			} else {
				// Secondary decryption
				std::vector<uint8_t> decryption2;
				try {
					// 二次解码
					decryption2 = Decrypt::PreDecrypt(tempFileByte.Byte, MataJson.name);

					// 移除 .cache 部分
					fixedName = MataJson.name.substr(0, MataJson.name.find(".cache"));

					// 检查 fixedName 是否以 .lua 结尾
					if (fixedName.size() >= 4 && fixedName.substr(fixedName.size() - 4) == ".lua") {
						// 更改后缀为 .luac
						fixedName.replace(fixedName.size() - 4, 4, ".luac");
					}
				} catch (const std::exception&) {
					std::cout << " ！Secondary decryption failed: " << MataJson.name << "\n";
					decryption2 = tempFileByte.Byte;
				}

				// Save file
				std::filesystem::path filePath2 = dirPath / fixedName;
				if (!std::filesystem::exists(filePath2)) {
					std::ofstream outFile(filePath2, std::ios::binary);
					outFile.write(reinterpret_cast<const char*>(decryption2.data()), decryption2.size());
					outFile.close();
				}
			}
		} else if (MataJson.type == "2") {
			// Create directory
			std::filesystem::path parentPath = MataJson.directoryPath;
			std::filesystem::path dirName = MataJson.name;
			std::filesystem::path dirPath = parentPath / dirName;
			if (!std::filesystem::exists(dirPath)) {
				std::filesystem::create_directories(dirPath);
				std::cout << " << Dir: " << MataJson.name << "\n";
			}
		} else {
			std::cout << " ！UNKONW:" << MataJson.name << "\n";
		}
	}

	/// <summary>
	/// 提取元信息并解码
	/// </summary>
	/// <param name="XorFileMatas">Xor文件元信息</param>
	void Mata::ExtractMateAndDecode(const nlohmann::json& XorFileMatas) {
		std::cout << "\n ！Extracting Mate And Decode......\n\n";

		// 遍历所有Map映射信息
		for (const auto& [key, values] : XorFileMatas.at("maps").items()) {
			if (values.empty()) {
				std::cout << " ！Empty map key: " << key << "\n";
				continue;
			}

			////////////////////////读取并拼接目录////////////////////////

			// 当前路径向量
			std::vector<std::string> currentDirPath;

			// 获取编码后的目录值 key -> "0x123"
			uint64_t blockOffsetHex = std::stoull(key.substr(2), nullptr, 16);

			// 判断是否是根目录 0 是根目录(可以使用FastXor.html验证)
			bool isRootDir = blockOffsetHex == 0;

			// 非根目录 与 List下存在编码后的key
			if (!isRootDir && XorFileMatas.at("list").contains(key)) {
				// 获取当前目录对象
				nlohmann::json currentDirObj = XorFileMatas.at("list")[key];

				// 判断是否为目录
				if (currentDirObj["type"] != "2") {
					std::cout << " ！Error! Not a directory\n";
					continue;
				}

				// 获取当前目录名称 -> currentDirPath向量
				currentDirPath.push_back(currentDirObj["name"]);
				// 获取16进制的编码后的偏移值
				uint64_t upDirBlockOOffsetHex = std::stoull(currentDirObj["blockEncodedOffset"].get<std::string>(), nullptr, 16);

				// 如果 > 0 则表示还有上级目录
				while (upDirBlockOOffsetHex > 0) {
					// 获取上级目录的编码偏移值
					std::string upOffset = currentDirObj["blockEncodedOffset"];

					// 判断是否存在
					if (XorFileMatas.at("list").contains(upOffset)) {
						// 存在
						// 获取上级目录对象
						currentDirObj = XorFileMatas.at("list")[upOffset];
						// 获取上级目录名称 -> currentDirPath向量
						currentDirPath.push_back(currentDirObj["name"]);
						// 获取上级目录的编码偏移值
						upDirBlockOOffsetHex = std::stoull(currentDirObj["blockEncodedOffset"].get<std::string>(), nullptr, 16);
					} else {
						// 不存在则放到 Other 目录下
						currentDirPath.push_back("Other");
						break;
					}
				}
			} else {
				// 非根目录
				if (!isRootDir) {
					// 编码偏移值 -> currentDirPath向量
					currentDirPath.push_back(key);
					// 放入Other目录
					currentDirPath.push_back("Other");
				}
			}

			// 当前正在处理的.xor文件名(无后缀) -> currentDirPath向量
			currentDirPath.push_back(GV::NowXor.Name);
			if (currentDirPath.size() > 1) {
				// 反转
				std::reverse(currentDirPath.begin(), currentDirPath.end());
			}

			// 拼接后的路径
			std::string dirTempPath;
			for (const auto& path : currentDirPath) {
				dirTempPath += path + "\\";
			}

			// 拼接后的绝对路径
			std::string filePath = (GV::ExeDir / dirTempPath).string();
			std::cout << " Dir: " << dirTempPath << "\n";

			////////////////////////遍历解码保存目录下的文件////////////////////////

			// 遍历目录下的文件
			for (const auto& value : values) {
				// 判断是否包含
				if (XorFileMatas.at("list").contains(value)) {
					// 获取信息
					nlohmann::json info = XorFileMatas.at("list")[value];
					// 提取信息
					MataSaveInfo saveInfo{
						info["type"],
						info["name"],
						std::stoull(info["decodedOffset"].get<std::string>(), nullptr, 16),
						std::stoull(info["size"].get<std::string>(), nullptr, 16),
						filePath
					};
					// 解码并保存文件
					Mata::DecodeAndSaveFile(saveInfo);
				}
			}
		}
		std::cout << "\n ！Extracting Mate And Decode completed!\n\n";
	}
}