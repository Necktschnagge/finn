#pragma once
#include "logger.h"

#include <nlohmann/json.hpp>

#include <string>

#include <fstream>

namespace {
	auto& utility_logger = standard_logger;
}

inline nlohmann::json load_json(const std::string& file_name, const nlohmann::json& default_json = nlohmann::json::object()) {
	std::ifstream ofile;
	ofile.open(file_name);
	if (ofile.good()) { // check if opened a file
		ofile >> std::noskipws;
		try {
			return nlohmann::json::parse(std::istream_iterator<std::ifstream::char_type>(ofile), std::istream_iterator<std::ifstream::char_type>());
		}
		catch (...) {
			utility_logger()->warn(std::string("Could not parse opened file named:   ").append(file_name));
		}
		ofile.close();
	}
	else {
		utility_logger()->warn(std::string("Could not open a file named:   ").append(file_name));
	}
	utility_logger()->info("Will return a default_json as specified.");
	return default_json;
}
