#pragma once
#include "logger.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <string>

class finnhub_rest_client final {
public:
	class response_json_error : public std::runtime_error {
	public:
		response_json_error(const std::string& error_message) : std::runtime_error(error_message) {}

		virtual const char* what() const noexcept override {
			return this->std::runtime_error::what();
		}
	};

private:
	std::string api_key;

	inline static nlohmann::json try_parse_api_response(const std::string& text, const cpr::Url& url) {
		try {
			return nlohmann::json::parse(text);
		}
		catch (...) {
			finnhub_client_logger()->error("Could not parse an http response as json.");
			finnhub_client_logger()->trace(text.size() > 100 ? text.substr(0, 100) : text);
			throw response_json_error(std::string("Could not parse a response from ").append(url.c_str()));
		}
	}

	inline static nlohmann::json ensured_finnhub_api_request(const cpr::Url& url, const cpr::Parameters& params) {
		using namespace std::chrono_literals;
		finnhub_client_logger()->trace(std::string("url:   ").append(url.c_str()));
	ensured_finnhub_api_request__again_request:
		cpr::Response r = cpr::Get(url, params);
		finnhub_client_logger()->trace(std::string("response status code:   ").append(std::to_string(r.status_code)));
		if (r.status_code == 429) {
			finnhub_client_logger()->debug("Encountered api limit exceed.Trying again...");
			std::this_thread::sleep_for(2s);
			goto ensured_finnhub_api_request__again_request;
		}
		finnhub_client_logger()->trace(std::string("response content type:   ").append(r.header["content-type"]));
		return try_parse_api_response(r.text, url);
	}

public:
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

