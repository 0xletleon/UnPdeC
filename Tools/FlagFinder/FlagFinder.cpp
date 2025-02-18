#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>

namespace fs = std::filesystem;

// 全局变量
std::map<uint8_t, std::vector<fs::path>> filesByFlag; // 按 Flag 分类存储文件路径
std::mutex dataMutex;                                // 保护 filesByFlag 的互斥锁
std::atomic<int> fileCount{ 0 };                       // 已处理的文件数量

// 处理单个文件
static void ProcessFile(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << "\n";
        return;
    }

    // 读取 0x18 处的 1 个字节
    file.seekg(0x18, std::ios::beg);
    uint8_t flag;
    file.read(reinterpret_cast<char*>(&flag), sizeof(flag));

    if (file) {
        // 加锁保护全局数据
        std::lock_guard<std::mutex> lock(dataMutex);
        filesByFlag[flag].push_back(filePath);
    } else {
        std::cerr << "Failed to read flag from file: " << filePath << "\n";
    }

    // 更新已处理的文件数量
    fileCount++;
}

// 处理一批文件
static void ProcessFiles(const std::vector<fs::path>& files) {
    std::vector<std::future<void>> futures;
    for (const auto& file : files) {
        // 使用异步任务处理每个文件
        futures.push_back(std::async(std::launch::async, ProcessFile, file));
    }

    // 等待所有异步任务完成
    for (auto& future : futures) {
        future.wait();
    }
}

// 递归读取目录下的 .cache 文件
static void ReadCacheFiles(const fs::path& directory) {
    try {
        // 收集所有 .cache 文件
        std::vector<fs::path> cacheFiles;
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cache") {
                cacheFiles.push_back(entry.path());
            }
        }

        // 多线程处理文件
        const size_t numThreads = std::thread::hardware_concurrency(); // 获取 CPU 核心数
        std::vector<std::thread> threads;
        size_t filesPerThread = cacheFiles.size() / numThreads;

        for (size_t i = 0; i < numThreads; ++i) {
            size_t start = i * filesPerThread;
            size_t end = (i == numThreads - 1) ? cacheFiles.size() : start + filesPerThread;
            threads.emplace_back(ProcessFiles, std::vector<fs::path>(cacheFiles.begin() + start, cacheFiles.begin() + end));
        }

        // 等待所有线程完成
        for (auto& thread : threads) {
            thread.join();
        }

        // 打印统计信息
        std::cout << "Processed " << fileCount << " files.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    // 要探索的目录
    fs::path directory;

    // 判断是否指定了目录，如果没有那么探索程序所在目录
    if (argc != 2) {
        // 程序当前目录
        directory = fs::current_path();
    } else {
        // 目标目录
        fs::path directory = argv[1];
    }
    std::cout << "Finding .cache files in directory: \n" << directory << "\n";

    // 目标目录是否存在
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Invalid directory: " << directory << "\n";
        return 1;
    }

    // 递归读取 .cache 文件
    ReadCacheFiles(directory);

    // 将结果写入文件
    std::ofstream outFile("FlagFinder_Result.txt");
    if (!outFile) {
        std::cerr << "Failed to create output file: FlagFinder_Result.txt\n";
        return 1;
    }

	// 先写入统计摘要
	outFile << "// Summary: ";
	for (auto it = filesByFlag.begin(); it != filesByFlag.end(); ++it) {
		outFile << "0x" << std::hex << static_cast<int>(it->first)
			<< ": " << std::dec << it->second.size() << ", ";
	}
	outFile << "\n\n";

    // 按 Flag 分类写入文件路径
    for (const auto& [flag, files] : filesByFlag) {
        outFile << "0x" << std::hex << static_cast<int>(flag) << ":\n";
        for (const auto& file : files) {
            outFile << file << "\n";
        }
        outFile << "\n"; // 每个分类之间空一行
    }

    std::cout << "Results written to FlagFinder_Result.txt\n";
    return 0;
}