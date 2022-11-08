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
			throw std::runtime_error("market_news_category does not know an enum class value.");
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

	inline nlohmann::json ensured_finnhub_api_request(const cpr::Url& url, cpr::Parameters& params) const {
		using namespace std::chrono_literals;
		cpr::Header header{ { "X-Finnhub-Token", api_key } };
		//params.Add({ "token", api_key }); <-- You can use this instead of passing the api_key inside the header.
		finnhub_client_logger()->trace(std::string("url:   ").append(url.c_str()));
	ensured_finnhub_api_request__again_request:
		cpr::Response r = cpr::Get(url, header, params);
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
	* https://finnhub.io/docs/api/symbol-search
	* You can find Sybols using the ISIN or company name etc. "symbol, name, isin, or cusip"
	*/
	nlohmann::json getSymbolLookup(const std::string& query_string) const;

	/**
	* https://finnhub.io/docs/api/stock-symbols
	*/
	nlohmann::json getStockSymbols(const std::string& exchange = "US") const;

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
	nlohmann::json getStockBasicFinancials(const std::string& symbol, const std::string& metric = "all") const;

	/**
	* https://finnhub.io/docs/api/quote
	*/
	nlohmann::json getQuotes(const std::string& symbol) const;

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


What are the intervals allowed for candle stick query? use the query tp build continuous chart´...

*/
