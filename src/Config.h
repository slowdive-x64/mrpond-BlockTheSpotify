#pragma once
#include <string>
#include <map>

class Config {
	
public:
	Config () {
		m_filename = "./config.ini";
		m_data["Log"] = read ("Config", "Log");
		m_data["Skip_wpad"] = read ("Config", "Skip_wpad");
		m_data["Block_BannerOnly"] = read ("Config", "Block_BannerOnly");
	}

	bool getConfig (std::string_view key) {
		auto find = m_data.find (key);
		if (find != m_data.end()) {
			return find->second;
		}
		return false;
	}
private:
	std::string_view m_filename;
	std::map<std::string_view, bool> m_data;

	bool read (const char* app, const char* key, int def_value = 0) {
		if (1 == GetPrivateProfileInt (app, key, def_value, m_filename.data())) {
			return true;
		}
		return false;
	}
};

