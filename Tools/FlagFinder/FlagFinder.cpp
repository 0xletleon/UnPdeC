#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <array>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <iomanip>

namespace fs = std::filesystem;

// 全局数据结构
struct {
	// Flag 分析数据
	std::map<uint8_t, std::vector<fs::path>> by_flag;
	std::mutex flag_mutex;

	// 头部8字节分析数据
	std::map<std::string, std::set<std::array<uint8_t, 8>>> by_suffix;
	std::mutex suffix_mutex;

	// 原子计数器
	std::atomic<int> processed_files{ 0 };
	std::atomic<int> total_headers{ 0 };
} analysis_data;

// 高效提取后缀名（如 scene.mesh.cache -> "mesh"）
static std::string extract_suffix(const fs::path& path) {
	std::string stem = path.stem().string();
	size_t pos = stem.find_last_of('.');
	return (pos != std::string::npos) ? stem.substr(pos + 1) : stem;
}

// 并行处理单个文件
static void process_file(const fs::path& file) {
	std::ifstream fs(file, std::ios::binary);
	if (!fs) {
		std::cerr << "无法打开文件: " << file << "\n";
		return;
	}

	// 同时进行两种分析
	try {
		// 分析1: 0x18处的flag
		uint8_t flag = 0;
		fs.seekg(0x18);
		if (fs.read(reinterpret_cast<char*>(&flag), 1)) {
			std::lock_guard lock(analysis_data.flag_mutex);
			analysis_data.by_flag[flag].push_back(file);
		}

		// 分析2: 前8字节
		std::array<uint8_t, 8> header{};
		fs.seekg(0);
		if (fs.read(reinterpret_cast<char*>(header.data()), 8)) {
			auto suffix = extract_suffix(file);
			std::lock_guard lock(analysis_data.suffix_mutex);
			auto& suffix_set = analysis_data.by_suffix.try_emplace(suffix).first->second;
			auto [it, inserted] = suffix_set.insert(header);  // 正确捕获临时对象
			if (inserted) analysis_data.total_headers++;
		}

		// 更新计数器
		analysis_data.processed_files++;
	} catch (const std::exception& e) {
		std::cerr << "处理文件错误 " << file << ": " << e.what() << "\n";
	}
}

// 批量处理任务
static void process_batch(const std::vector<fs::path>& batch) {
	std::vector<std::future<void>> futures;
	futures.reserve(batch.size());  // 预分配避免多次分配

	for (const auto& file : batch) {
		futures.emplace_back(std::async(std::launch::async, process_file, file));
	}

	for (auto& f : futures) f.wait();
}

// 递归查找.cache文件
static std::vector<fs::path> find_cache_files(const fs::path& dir) {
	std::vector<fs::path> files;
	try {
		for (const auto& entry : fs::recursive_directory_iterator(dir)) {
			if (entry.is_regular_file() && entry.path().extension() == ".cache") {
				files.push_back(entry.path());
			}
		}
	} catch (const std::exception& e) {
		std::cerr << "目录遍历错误: " << e.what() << "\n";
	}
	return files;
}

int main(int argc, char** argv) {
	std::cout << R"(
==================================================================
    FlagFinder Analyzer - 文件特征分析工具 @letleon
==================================================================
功能说明：
1. 递归扫描指定目录的.cache文件
2. 双重分析模式：
    - 特征标记分析：提取0x18偏移处的1字节标志
    - 头部指纹分析：采集文件前8字节作为唯一特征
3. 自动生成精简路径格式的报表
4. 支持多线程高速处理（自动检测CPU核心数）

Features:
1. Recursively scan .cache files in target directory
2. Dual analysis mode:
    - Signature analysis: Extract 1-byte flag at 0x18 offset
    - Header fingerprint: Collect unique 8-byte file headers
3. Generate reports with compact relative paths
4. Multi-threaded processing (Auto-detects CPU cores)

Ai assistant powered by: SeepSeek R1
)";

	// 参数处理
	const fs::path root_dir = (argc > 1) ? argv[1] : fs::current_path();

	if (!fs::exists(root_dir)) {
		std::cerr << "目录不存在: " << root_dir << "\n";
		return 1;
	}

	// 阶段1: 查找所有.cache文件
	std::cout << "正在扫描.cache文件..." << std::endl;
	auto all_files = find_cache_files(root_dir);
	if (all_files.empty()) {
		std::cout << "未找到.cache文件" << std::endl;
		return 0;
	}

	// 阶段2: 并行处理
	const unsigned num_cores = std::thread::hardware_concurrency();
	const size_t batch_size = std::max(all_files.size() / (num_cores * 4), size_t(1));  // 更细粒度批处理

	std::vector<std::thread> workers;
	auto it = all_files.begin();

	// 动态分配任务
	for (size_t i = 0; i < num_cores * 2; ++i) {
		if (it >= all_files.end()) break;

		auto end = it + std::min(batch_size, size_t(all_files.end() - it));
		workers.emplace_back(process_batch, std::vector<fs::path>(it, end));
		it = end;
	}

	// 等待所有线程完成
	for (auto& t : workers) t.join();

	// 输出统计信息
	std::cout << "\n处理完成:\n"
		<< "总文件数: " << analysis_data.processed_files << "\n"
		<< "唯一头数量: " << analysis_data.total_headers << "\n"
		<< "不同Flag类型: " << analysis_data.by_flag.size() << "\n"
		<< "不同后缀类型: " << analysis_data.by_suffix.size() << "\n";

	// 写入Flag结果
	std::ofstream flag_out("Flag0x18.txt");
	for (const auto& [flag, paths] : analysis_data.by_flag) {
		flag_out << "0x" << std::hex << int(flag) << " (" << std::dec << paths.size() << "):\n";
		for (const auto& p : paths) {
			// 将绝对路径转换为相对路径[7](@ref)
			flag_out << fs::relative(p, root_dir).generic_string() << "\n"; // 使用generic_string确保跨平台斜杠
		}
		flag_out << "\n";
	}

	// 写入头部数据
	std::ofstream head_out("head8byte.txt");
	for (const auto& [suffix, headers] : analysis_data.by_suffix) {
		head_out << "[" << suffix << "]\n";
		for (const auto& h : headers) {
			for (size_t i = 0; i < 8; ++i) {
				head_out << std::hex << std::setw(2) << std::setfill('0')
					<< int(h[i]);
				if (i == 3) head_out << " ";
			}
			head_out << "\n";
		}
		head_out << "\n";
	}

	std::cout << "结果已保存至 Flag0x18.txt 和 head8byte.txt\n";
	return 0;
}