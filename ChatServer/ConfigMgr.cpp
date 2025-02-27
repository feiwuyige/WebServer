#include "ConfigMgr.h"

std::string ConfigMgr::GetValue(const std::string& section, const std::string& key)
{
	if (_config_map.find(section) == _config_map.end()) {
		return "";
	}

	return _config_map[section].GetValue(key);
}

ConfigMgr::ConfigMgr()
{
	//使用boost管理文件读取
	boost::filesystem::path current_path = boost::filesystem::current_path();
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "Config_Path: " << config_path << std::endl;
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);
	for (const auto& section_pair : pt) {
		const ::std::string& section_name = section_pair.first;
		const boost::property_tree::ptree& section_tree = section_pair.second;
		std::map<std::string, std::string> section_config;
		for (const auto& key_value_pair : section_tree) {
			const std::string& key = key_value_pair.first;
			const std::string& value = key_value_pair.second.get_value<std::string>();
			section_config[key] = value;
		}
		SectionInfo section_info;
		section_info._section_datas = section_config;
		_config_map[section_name] = section_info;
	}
}
