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

	nlohmann::json getStockSymbols(const std::string& exchange = "US") {
		finnhub_client_logger()->debug(std::string("Getting stock symbols...   [").append(exchange).append("]"));
		const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/symbol" } };
		const auto params{ cpr::Parameters{ {"exchange", exchange}, {"token", api_key} } };
		finnhub_client_logger()->trace(std::string("url:   ").append(url.c_str()));
		cpr::Response r = cpr::Get(url, params);
		finnhub_client_logger()->trace(std::string("response status code:   ").append(std::to_string(r.status_code)));
		finnhub_client_logger()->trace(std::string("response content type:   ").append(r.header["content-type"]));
		try {
			nlohmann::json stock_list = nlohmann::json::parse(r.text);
			return stock_list;
		}
		catch (...) {
			finnhub_client_logger()->error("Could not parse an http response as json.");
			finnhub_client_logger()->trace(r.text);
			throw response_json_error(std::string("Could not parse response from ").append(url.c_str()));
		}
	}
};

