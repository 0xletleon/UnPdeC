#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <Windows.h>
#include "Encrypt.h"

namespace fs = std::filesystem;

std::vector<uint8_t> ReadFile(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error(std::format("无法打开文件: {}", filePath.string()));
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        throw std::runtime_error(std::format("读取文件失败: {}", filePath.string()));
    }

    return buffer;
}

void WriteFile(const fs::path& filePath, const std::vector<uint8_t>& data) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error(std::format("无法创建文件: {}", filePath.string()));
    }

    if (!file.write(reinterpret_cast<const char*>(data.data()), data.size())) {
        throw std::runtime_error(std::format("写入文件失败: {}", filePath.string()));
    }
}

void ProcessFile(const fs::path& inputPath) {
    try {
        fs::path outputPath = inputPath;
        outputPath += ".cache";

        std::cout << "正在读取文件: " << inputPath.string() << std::endl;
        auto inputData = ReadFile(inputPath);

        std::cout << "正在加密..." << std::endl;
        auto encryptedData = UnPdeC::Encrypt::PreEncrypt(inputData);

        std::cout << "正在写入文件: " << outputPath.string() << std::endl;
        WriteFile(outputPath, encryptedData);

        std::cout << "加密完成!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        // 设置控制台编码为UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        if (argc < 2) {
            std::cout << "用法: EnCache.exe <文件路径>\n";
            std::cout << "或直接将文件拖放到程序上\n";
            return 1;
        }

        for (int i = 1; i < argc; ++i) {
            fs::path inputPath = argv[i];

            if (!fs::exists(inputPath)) {
                std::cerr << "文件不存在: " << inputPath.string() << std::endl;
                continue;
            }

            if (!fs::is_regular_file(inputPath)) {
                std::cerr << "不是普通文件: " << inputPath.string() << std::endl;
                continue;
            }

            ProcessFile(inputPath);
        }

        // 如果是直接双击运行，等待用户按键后退出
        if (argc > 1 && !GetConsoleWindow()) {
            std::cout << "\n按任意键退出...";
            std::cin.get();
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "程序发生错误: " << e.what() << std::endl;
        return 1;
    }
}