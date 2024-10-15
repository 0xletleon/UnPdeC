#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <windows.h>

namespace fs = std::filesystem;

static void showLicenseInfo() {
	std::cout << "UnLuacBat v1.0 By:letleon\n\n";
	std::cout << "Compile the modified UnLuac into a standalone .exe program using Oracle GraalVM\n";
	std::cout << "This program was completed with the assistance of Tongyi QianWen 2.5\n";
	std::cout << "This program is intended for educational and research purposes only and may not be used for commercial purposes.\n\n";
	std::cout << "unluac author: tehtmi\n";
	std::cout << "Repository: https://sourceforge.net/projects/unluac\n\n";
	std::cout << "AvatarStarCBT author: AeonLucid\n";
	std::cout << "Repository: https://github.com/AeonLucid/AvatarStarCBT\n\n";
}

static void waitForTwoSeconds() {
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main(int argc, char* argv[]) {
	showLicenseInfo();

	// Check if input file is provided
	if (argc != 2) {
		std::cout << "No input file provided.\n\nUsage: UnLuacBat <input_file.luac or input_file.lua>\n";
		return 1;
	}

	std::string input_file = argv[1];

	// Check if file exists
	if (!fs::exists(input_file)) {
		std::cout << "File not found: " << input_file << "\n\nPress any key to close the console...";
		return 1;
	}

	// Check if file extension is .luac or .lua
	if (fs::path(input_file).extension() != ".luac" && fs::path(input_file).extension() != ".lua") {
		std::cout << "The input file must be a .luac or .lua file." << "\n\nPress any key to close the console...";
		return 1;
	}

	// Set input and output files
	fs::path input_path(input_file);
	fs::path output_file = input_path.parent_path() / (input_path.stem().string() + "_d.lua");

	// Get the directory of the executable
	fs::path exe_dir = fs::absolute(fs::path(argv[0])).parent_path();

	// Construct the full path to UnLuacGVM.exe
	fs::path unluacgvm_exe = exe_dir / "UnLuacGVM.exe";

	// Check if UnLuacGVM.exe exists
	if (!fs::exists(unluacgvm_exe)) {
		std::cout << "UnLuacGVM.exe not found at:" << unluacgvm_exe.string() << "\n\nPress any key to close the console...";
		return 1;
	}

	// Process .luac or .lua file
	std::cout << " ！Input file: " << input_file << "\n";

	// Construct the command to run UnLuacGVM.exe
	std::string command = "UnLuacGVM.exe --lua_version 5.2-alpha \"" + input_file + "\" > \"" + output_file.string() + "\"";
	int result = std::system(command.c_str());

	if (result != 0) {
		std::cout << " ！Failed to process " << input_file << ". Error code: " << result << "\n";
		return 1;
	}

	std::cout << "√ Success " << output_file.string() << "\n";

	// Wait for 2 seconds before closing the console
	waitForTwoSeconds();

	return 0;
}