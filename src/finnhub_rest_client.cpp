#include "finnhub_rest_client.h"

nlohmann::json finnhub_rest_client::getStockSymbols(const std::string& exchange) {
	finnhub_client_logger()->debug(std::string("Getting stock symbols...   [").append(exchange).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/symbol" } };
	const auto params{ cpr::Parameters{ { "exchange", exchange },{ "token", api_key } } };
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

nlohmann::json finnhub_rest_client::getStockProfile2(const std::string& symbol) {
	finnhub_client_logger()->debug(std::string("Getting stock profile2...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/profile2" } };
	finnhub_client_logger()->trace(std::string("url:   ").append(url.c_str()));
	const auto params{
		cpr::Parameters{
			{ "symbol", symbol },
			{ "token", api_key }
		}
	};
	cpr::Response r = cpr::Get(url, params);
	finnhub_client_logger()->trace(std::string("response status code:   ").append(std::to_string(r.status_code)));
	finnhub_client_logger()->trace(std::string("response content type:   ").append(r.header["content-type"]));

	try {
		nlohmann::json profile2 = nlohmann::json::parse(r.text);
		return profile2;
	}
	catch (...) {
		finnhub_client_logger()->error("Could not parse an http response as json.");
		finnhub_client_logger()->trace(r.text);
		throw response_json_error(std::string("Could not parse response from ").append(url.c_str()));
	}
}

nlohmann::json finnhub_rest_client::getStockCandles(const std::string& symbol, uint64_t from, uint64_t to, uint64_t resolution) {
	finnhub_client_logger()->debug(std::string("Getting stock candles...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/candle" } };
	finnhub_client_logger()->trace(std::string("url:   ").append(url.c_str()));
	const auto params{
		cpr::Parameters{
			{ "symbol", symbol },
			{ "resolution", std::to_string(resolution) },
			{ "from", std::to_string(from) },
			{ "to", std::to_string(to) },
			{ "token", api_key }
		}
	};
	cpr::Response r = cpr::Get(url, params);
	finnhub_client_logger()->trace(std::string("response status code:   ").append(std::to_string(r.status_code)));
	finnhub_client_logger()->trace(std::string("response content type:   ").append(r.header["content-type"]));

	try {
		nlohmann::json candles = nlohmann::json::parse(r.text);
		return candles;
	}
	catch (...) {
		finnhub_client_logger()->error("Could not parse an http response as json.");
		finnhub_client_logger()->trace(r.text);
		throw response_json_error(std::string("Could not parse response from ").append(url.c_str()));
	}
}

nlohmann::json finnhub_rest_client::getQuotes(const std::string& symbol) {
	finnhub_client_logger()->debug(std::string("Getting quotes...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/quote" } };
	finnhub_client_logger()->trace(std::string("url:   ").append(url.c_str()));
	const auto params{
		cpr::Parameters{
			{ "symbol", symbol },
			{ "token", api_key }
		}
	};
	cpr::Response r = cpr::Get(url, params);
	finnhub_client_logger()->trace(std::string("response status code:   ").append(std::to_string(r.status_code)));
	finnhub_client_logger()->trace(std::string("response content type:   ").append(r.header["content-type"]));

	try {
		nlohmann::json quote = nlohmann::json::parse(r.text);
		return quote;
	}
	catch (...) {
		finnhub_client_logger()->error("Could not parse an http response as json.");
		finnhub_client_logger()->trace(r.text);
		throw response_json_error(std::string("Could not parse response from ").append(url.c_str()));
	}
}
