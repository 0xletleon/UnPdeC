#include "OffsetLog.h"

namespace UnPdeC {

	nlohmann::json OffsetLog::LogData;

	void OffsetLog::InitializeLogDataIfNeeded() {
		if (LogData.empty()) {
			LogData = {
				{GV::NowPde.Name, {
					{"_info", {
						{"Type", 2},
						{"Name", "Root"},
						{"BlockOffset", "0"},
						{"OriginalOffset", "0"},
						{"PdeOffset", "1000"},
						{"Size", "1000"}
					}},
					{"Files", nlohmann::json::array()}
				}}
			};
		}
	}

	std::string OffsetLog::ToHex(uint32_t value) {
		auto hex = std::format("{:X}", value);
		return std::string(std::find_if(hex.begin(), hex.end(), [](char c) { return c != '0'; }), hex.end());
	}

	void OffsetLog::AddEntry(uint32_t BlockOffset, uint32_t OriginalOffset, uint32_t PdeOffset, uint32_t Size,
		std::string_view Name, uint8_t Type, std::string_view Path) {
		InitializeLogDataIfNeeded();

		nlohmann::json Entry = {
			{"Type", Type},
			{"Name", Name},
			{"BlockOffset", ToHex(BlockOffset)},
			{"OriginalOffset", ToHex(OriginalOffset)},
			{"PdeOffset", ToHex(PdeOffset)},
			{"Size", ToHex(Size)}
		};

		if (Type == 1) { // File
			nlohmann::json& Node = GetOrCreatePath(Path);
			Node["Files"].push_back(Entry);
		} else if (Type == 2) { // Directory
			std::string FullPath = Path.empty() ? std::string(Name) : std::string(Path) + "\\" + std::string(Name);
			nlohmann::json& Node = GetOrCreatePath(FullPath);
			Node["_info"] = Entry;
			if (Node.find("Files") == Node.end()) {
				Node["Files"] = nlohmann::json::array();
			}
		}
	}

	nlohmann::json& OffsetLog::GetOrCreatePath(std::string_view Path) {
		nlohmann::json* Current = &LogData;

		for (const auto& Part : std::views::split(Path, '\\') | std::views::transform([](auto&& rng) { return std::string_view(&*rng.begin(), std::ranges::distance(rng)); })) {
			if (Current->find(Part) == Current->end()) {
				(*Current)[std::string(Part)] = {
					{"Files", nlohmann::json::array()}
				};
			}
			Current = &(*Current)[std::string(Part)];
		}

		return *Current;
	}

	void OffsetLog::SaveToFile() {
		std::filesystem::path logFilePath = GV::ExeDir / GV::NowPde.Name / "offsetlog.json";
		std::ofstream File(logFilePath);
		if (File.is_open()) {
			File << std::setw(4) << LogData << std::endl;
		} else {
			throw std::runtime_error("无法打开日志文件进行写入");
		}
	}

	void OffsetLog::WriteToGV() {
		GV::OffsetLogResult = LogData;
		std::cout << "偏移日志数据已写入 GV::OffsetLogResult" << std::endl;
	}

} // namespace UnPdeC