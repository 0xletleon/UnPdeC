#include "Tools.h"

namespace UnPdeC {
	void Tools::Init() {
		std::cout << " ！正在初始化Tools\n";

		// 检查目录是否存在
		std::filesystem::path XorDir = GV::ExeDir / GV::NowXor.Name;
		if (!std::filesystem::exists(XorDir)) {
			// 目录不存在，创建它
			try {
				std::filesystem::create_directory(XorDir);
				std::cout << " << 已创建目录: " << XorDir << "\n";
			} catch (const std::exception& e) {
				std::cerr << " ！创建目录失败: " << e.what() << "\n";
				return;
			}
		}

		std::cout << " √成功初始化Tools\n";
	}

	// 获取所有.xor文件
	std::vector<TNowXor> Tools::FindXorFile() {
		// xor文件名数组
		std::vector<TNowXor> XorArr;

		// 获取可执行文件所在目录
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::filesystem::path exePath = std::filesystem::path(buffer).remove_filename();

		std::cout << " ！程序所在目录：" << exePath.string() << "\n";

		GV::ExeDir = exePath;// 将程序目录写入全局变量

		// 遍历可执行文件所在目录，查找所有 .xor 文件
		for (const auto& entry : std::filesystem::directory_iterator(exePath)) {
			if (entry.path().extension() == ".xor") {
				// 当前xor信息
				TNowXor nowXor;

				// 从path对象获取文件名，并去除后缀名
				std::string fileName = entry.path().filename().string();
				fileName = fileName.substr(0, fileName.find_first_of('.'));
				nowXor.Name = fileName;

				nowXor.Size = std::filesystem::file_size(entry.path());// 获取当前文件的大小

				nowXor.Path = entry.path().string();// 获取当前xor文件的路径

				nowXor.JsonPath = (exePath / fileName).string() + ".json";// 获取当前xor文件的json路径

				std::cout << " ！Path: " << nowXor.Path << "\n ！Name: " << nowXor.Name << " Size: " << nowXor.Size << "\n";

				XorArr.push_back(nowXor);
			}
		}

		// 如果没有找到.xor文件，则退出程序
		if (XorArr.empty()) {
			std::cout << " ！目录下没有找到 *.xor文件！\n 按任意键退出程序\n";
			std::cin.get();
			exit(-1);
		}

		// 返回.xor文件名列表
		return XorArr;
	}

	// 将16进制转换为16进制字符串 HEX -> "0x1234"
	std::string Tools::ToHexString(uint64_t value) {
		std::stringstream stream;
		stream << "0x" << std::uppercase << std::hex << value;
		return stream.str();
	}

	/// <summary>
	/// 原始偏移值 -> 实际偏移值
	/// </summary>
	/// <param name="OrigOffset">原始偏移值</param>
	/// <returns>实际偏移值</returns>
	uint64_t Tools::OrigToPdeOffset(uint64_t OrigOffset) {
		return ((OrigOffset >> 10) + OrigOffset + 1) << 12;
	}

	/// <summary>
	/// 实际偏移值 -> 原始偏移值
	/// </summary>
	/// <param name="PdeOffset">实际偏移值</param>
	/// <returns>原始偏移值</returns>
	uint64_t Tools::PdeToOrigOffset(uint64_t PdeOffset) {
		return (PdeOffset >> 12) * 1024 / 1025;
	}

	/// <summary>
	/// 名称验证
	/// </summary>
	/// <param name="name">名称</param>
	/// <returns>是否合法</returns>
	bool Tools::NameCheck(const std::string& name) {
		if (name.empty() || name.back() == ' ' || name.back() == '.') {
			return false;
		}
		return std::all_of(name.begin(), name.end(), [](char ch) {
			return (ch > 0x20 && ch < 0x7F) && ch != '\"' && ch != '*' && ch != '/' &&
				ch != ':' && ch != '<' && ch != '>' && ch != '?' && ch != '\\' && ch != '|';
		});
	}

	/// <summary>
	/// 从xor文件中获取指定块数据
	/// </summary>
	/// <param name="Start">块在xor文件中的起始偏移</param>
	/// <param name="Size">块大小</param>
	/// <returns>块数据</returns>
	GetOffsetStr Tools::GetByteOfXor(uint64_t Start, uint64_t Size) {
		// 打开文件用于读取二进制数据
		std::ifstream XORFILE_FS(GV::NowXor.Path, std::ios::binary);

		// 检查文件是否打开成功
		if (!XORFILE_FS) {
			std::cerr << "无法打开xor文件: " << GV::NowXor.Name << "\n";
			// 退出程序
			exit(1);
		}

		// 设置读取起点
		XORFILE_FS.seekg(Start, std::ios::beg);

		// 检查文件位置是否正确
		if (XORFILE_FS.tellg() != Start) {
			std::cerr << "文件定位失败\n";
			return { 0, std::vector<uint8_t>() };
		}

		std::vector<uint8_t> buffer(Size);
		XORFILE_FS.read(reinterpret_cast<char*>(buffer.data()), Size);

		if (!XORFILE_FS) {
			std::cerr << "读取PDE文件失败\n";
			// 如果读取失败，返回空的GetOffsetStr
			return { 0, std::vector<uint8_t>() };
		}

		return { static_cast<uint64_t>(XORFILE_FS.gcount()), buffer };
	}
}