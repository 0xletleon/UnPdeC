#include "Decrypt.h"

static std::vector<uint8_t> readFile(const std::filesystem::path& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		throw std::runtime_error("Unable to open file: " + path.string());
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(size);
	if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
		return buffer;
	} else {
		throw std::runtime_error("Failed to read file: " + path.string());
	}
}

static void writeFile(const std::filesystem::path& path, const std::vector<uint8_t>& data) {
	std::ofstream outFile(path, std::ios::binary);
	if (!outFile) {
		throw std::runtime_error("Unable to open file for writing: " + path.string());
	}
	outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
}

int main(int argc, char* argv[]) {
	std::cout << "Hello World!\n";

	if (argc != 2) {
		std::cout << "No input file provided.\n";
		return 1;
	}

	std::string input_file = argv[1];

	if (!std::filesystem::exists(input_file)) {
		std::cout << "File not found: " << input_file << "\n";
		return 1;
	}

	if (std::filesystem::path(input_file).extension() != ".cache") {
		std::cout << "The input file must be a .cache file.\n";
		return 1;
	}

	try {
		auto file_path = std::filesystem::path(input_file);
		auto filename = file_path.filename().string();
		auto directory = file_path.parent_path();
		std::cout << "Filename: " << filename << "\n";
		std::cout << "Directory: " << directory << "\n";

		auto buffer = readFile(input_file);

		std::vector<uint8_t> decryption2;
		std::string fixedName;
		try {
			decryption2 = UnCache::Decrypt::PreDecrypt(buffer, filename);
			fixedName = filename.substr(0, filename.find(".cache"));
			if (fixedName.size() >= 4 && fixedName.substr(fixedName.size() - 4) == ".lua") {
				fixedName.replace(fixedName.size() - 4, 4, ".luac");
			}
		} catch (const std::exception&) {
			std::cout << "Secondary decryption failed: " << filename << "\n";
			fixedName = filename;
			decryption2 = buffer;
		}

		auto filePath2 = directory / fixedName;
		if (!std::filesystem::exists(filePath2)) {
			writeFile(filePath2, decryption2);
		}

		std::cout << "√ Success \n";
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}