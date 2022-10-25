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

	/**
	* https://finnhub.io/docs/api/stock-symbols
	*/
	nlohmann::json getStockSymbols(const std::string& exchange = "US");
	
	/**
	* https://finnhub.io/docs/api/company-profile2
	*/
	nlohmann::json finnhub_rest_client::getStockProfile2(const std::string& symbol);

	/**
	* https://finnhub.io/docs/api/stock-candles
	*/
	nlohmann::json getStockCandles(const std::string& symbol, uint64_t from, uint64_t to, uint64_t resolution);

	/**
	* https://finnhub.io/docs/api/quote
	*/
	nlohmann::json getQuotes(const std::string& symbol);


};

