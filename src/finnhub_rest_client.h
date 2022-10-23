#pragma once
#include "logger.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>

class finnhub_rest_client final {
private:
	std::string api_key;
public:
	class response_json_error : public std::runtime_error {
	public:
		response_json_error(const std::string& error_message) : std::runtime_error(error_message) {}

		virtual const char* what() const noexcept override {
			return this->std::runtime_error::what();
		}
	};

	finnhub_rest_client(const std::string& api_key) : api_key(api_key) {}

	nlohmann::json getStockSymbols(const std::string& exchange = "US");

	nlohmann::json getStockCandles(const std::string& symbol, uint64_t from, uint64_t to, uint64_t resolution);

};

