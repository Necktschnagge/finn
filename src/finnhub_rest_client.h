#pragma once
#include "logger.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <string>
#include <vector>

class finn_meta_data {
	friend class finnhub_rest_client;

	bool enable_response_codes{ true };

	std::vector<uint16_t> response_codes;
};

class finnhub_rest_client final {
public:
	class response_json_error : public std::runtime_error {
	public:
		response_json_error(const std::string& error_message) : std::runtime_error(error_message) {}

		virtual const char* what() const noexcept override {
			return this->std::runtime_error::what();
		}
	};

	class market_news_category {
	public:
		enum class values {
			general = 0,
			forex = 1,
			crypto = 2,
			merger = 3
		};
	private:
		values v;
	public:
		market_news_category(const values& value) : v(value) {}

		operator std::string() const {
			switch (v) {
			case values::general:
				return "general";
			case values::forex:
				return "forex";
			case values::crypto:
				return "crypto";
			case values::merger:
				return "merger";
			}
			return market_news_category(values::general);
			//throw std::runtime_error("market_news_category does not know an enum class value.");
		}
	};

private:
	std::string api_key;
	std::unique_ptr<finn_meta_data> meta_data;

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

	inline nlohmann::json ensured_finnhub_api_request(const cpr::Url& url, cpr::Parameters& params) const {
		using namespace std::chrono_literals;
		uint8_t max_repeat{ 100 };
		cpr::Header header{ { "X-Finnhub-Token", api_key } };
		//params.Add({ "token", api_key }); <-- You can use this instead of passing the api_key inside the header.
		finnhub_client_logger()->trace(std::string("url:   ").append(url.c_str()));
		//finnhub_client_logger()->trace(std::string("parameters:   ").append(params.);
	ensured_finnhub_api_request__again_request:
		--max_repeat;
		if (!max_repeat) {
			nlohmann::json error_message = { {"error", "Critical: After MAX_TRIES it was not possible to get a 200 server answer."} };
			return try_parse_api_response(error_message.dump(1), url);
		}
		cpr::Response r = cpr::Get(url, header, params);
		if (meta_data && meta_data->enable_response_codes) {
			meta_data->response_codes.push_back(static_cast<uint16_t>(r.status_code));
		}
		finnhub_client_logger()->trace(std::string("response status code:   ").append(std::to_string(r.status_code)));
		finnhub_client_logger()->trace(std::string("response content type:   ").append(r.header["content-type"]));
		if (r.status_code == 429) {
			finnhub_client_logger()->debug("Encountered api limit exceed (HTTP 429).Trying again...");
			std::this_thread::sleep_for(2s);
			goto ensured_finnhub_api_request__again_request;
		}
		if (r.status_code == 0) {
			finnhub_client_logger()->debug("Encountered server host not found (HTTP 0).Trying again...");
			std::this_thread::sleep_for(100ms);
			goto ensured_finnhub_api_request__again_request;
		}
		if (r.status_code == 502) {
			finnhub_client_logger()->debug("Encountered bad gateway (HTTP 502).Trying again...");
			std::this_thread::sleep_for(100ms);
			goto ensured_finnhub_api_request__again_request;
		}
		if (r.status_code != 200) {
			finnhub_client_logger()->error("Encountered HTTP status code without specific handler.Trying again...");
			finnhub_client_logger()->error(std::string("response status code:   ").append(std::to_string(r.status_code)));
			finnhub_client_logger()->error(std::string("response content type:   ").append(r.header["content-type"]));
			std::this_thread::sleep_for(3000ms);
			goto ensured_finnhub_api_request__again_request;
		}
		return try_parse_api_response(r.text, url);
	}

public:
	finnhub_rest_client(const std::string& api_key) : api_key(api_key) {
		meta_data = std::make_unique<finn_meta_data>();
	}

	const std::vector<uint16_t>& export_meta_status_codes() const {
		if (!meta_data)
			throw std::runtime_error("meta disabled");
		return meta_data->response_codes;
	}

	nlohmann::json export_meta_status_codes_json() const {
		if (!meta_data)
			return nlohmann::json::array();
		return nlohmann::json(meta_data->response_codes);
	}

	/**
	* https://finnhub.io/docs/api/symbol-search
	* You can find Sybols using the ISIN or company name etc. "symbol, name, isin, or cusip"
	*/
	nlohmann::json getSymbolLookup(const std::string& query_string) const;

	/**
	* https://finnhub.io/docs/api/stock-symbols
	*/
	nlohmann::json getStockSymbol(const std::string& exchange = "US") const;

	/**
	* https://finnhub.io/docs/api/company-profile2
	*/
	nlohmann::json getCompanyProfile2(const std::string& symbol) const;

	/**
	* https://finnhub.io/docs/api/market-news
	*/
	nlohmann::json getNews(const market_news_category& category, uint64_t min_id = 0) const;

	/**
	* https://finnhub.io/docs/api/company-basic-financials
	*/
	nlohmann::json getBasicFinancials(const std::string& symbol, const std::string& metric = "all") const;

	/**
	* https://finnhub.io/docs/api/quote
	*/
	nlohmann::json getQuote(const std::string& symbol) const;

	/**
	* https://finnhub.io/docs/api/stock-candles
	*/
	nlohmann::json getStockCandles(const std::string& symbol, uint64_t from, uint64_t to, uint64_t resolution) const;


};

/**
 API endpoint not yet tested:

Webhook https://finnhub.io/docs/api/webhook
Trades - Last Price Updates https://finnhub.io/docs/api/websocket-trades
Symbol Lookup https://finnhub.io/docs/api/symbol-search -> done
Company News https://finnhub.io/docs/api/company-news
Company Peers https://finnhub.io/docs/api/company-peers
Basic Financials https://finnhub.io/docs/api/company-basic-financials -> done
Insider Transactions https://finnhub.io/docs/api/insider-transactions
Insider Sentiment https://finnhub.io/docs/api/insider-sentiment
Financials As Reported https://finnhub.io/docs/api/financials-reported
SEC Filings https://finnhub.io/docs/api/filings
IPO Calendar https://finnhub.io/docs/api/ipo-calendar
Earnings Surprises https://finnhub.io/docs/api/company-earnings
Earnings Calendar https://finnhub.io/docs/api/earnings-calendar


Crypto Candles https://finnhub.io/docs/api/crypto-candles
Technical Indicators https://finnhub.io/docs/api/technical-indicator
Social Sentiment https://finnhub.io/docs/api/social-sentiment


What are the intervals allowed for candle stick query? use the query tp build continuous chart�...

*/
