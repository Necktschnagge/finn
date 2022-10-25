#include "finnhub_rest_client.h"

nlohmann::json finnhub_rest_client::getStockSymbols(const std::string& exchange) {
	finnhub_client_logger()->debug(std::string("Getting stock symbols...   [").append(exchange).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/symbol" } };
	const auto params{ cpr::Parameters{ { "exchange", exchange },{ "token", api_key } } };

	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getStockProfile2(const std::string& symbol) {
	finnhub_client_logger()->debug(std::string("Getting stock profile2...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/profile2" } };
	const auto params{
		cpr::Parameters{
			{ "symbol", symbol },
			{ "token", api_key }
		}
	};
	
	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getStockCandles(const std::string& symbol, uint64_t from, uint64_t to, uint64_t resolution) {
	finnhub_client_logger()->debug(std::string("Getting stock candles...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/candle" } };
	const auto params{
		cpr::Parameters{
			{ "symbol", symbol },
			{ "resolution", std::to_string(resolution) },
			{ "from", std::to_string(from) },
			{ "to", std::to_string(to) },
			{ "token", api_key }
		}
	};

	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getQuotes(const std::string& symbol) {
	finnhub_client_logger()->debug(std::string("Getting quotes...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/quote" } };
	const auto params{
		cpr::Parameters{
			{ "symbol", symbol },
			{ "token", api_key }
		}
	};

	return ensured_finnhub_api_request(url, params);
}
