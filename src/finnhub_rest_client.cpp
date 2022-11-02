#include "finnhub_rest_client.h"

nlohmann::json finnhub_rest_client::getSymbolLookup(const std::string& query_string) const
{
	finnhub_client_logger()->debug(std::string("Searching for shares...   [").append(query_string).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/search" } };
	auto params{
		cpr::Parameters{ { "q", query_string } }
	};

	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getStockSymbols(const std::string& exchange) const {
	finnhub_client_logger()->debug(std::string("Getting stock symbols...   [").append(exchange).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/symbol" } };
	auto params{
		cpr::Parameters{ { "exchange", exchange } }
	};

	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getCompanyProfile2(const std::string& symbol) const {
	finnhub_client_logger()->debug(std::string("Getting stock profile2...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/profile2" } };
	auto params{
		cpr::Parameters{
			{ "symbol", symbol }
		}
	};

	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getNews(const market_news_category& category, uint64_t min_id) const {
	finnhub_client_logger()->debug(std::string("Getting market news...   [").append(static_cast<std::string>(category)).append(",").append(std::to_string(min_id)).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/news" } };
	auto params{
		cpr::Parameters{
			{ "category", category },
			{ "minId", std::to_string(min_id)}
		}
	};

	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getStockCandles(const std::string& symbol, uint64_t from, uint64_t to, uint64_t resolution) const {
	finnhub_client_logger()->debug(std::string("Getting stock candles...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/stock/candle" } };
	auto params{
		cpr::Parameters{
			{ "symbol", symbol },
			{ "resolution", std::to_string(resolution) },
			{ "from", std::to_string(from) },
			{ "to", std::to_string(to) }
		}
	};

	return ensured_finnhub_api_request(url, params);
}

nlohmann::json finnhub_rest_client::getQuotes(const std::string& symbol) const {
	finnhub_client_logger()->debug(std::string("Getting quotes...   [").append(symbol).append("]"));
	const auto url{ cpr::Url{ "https://finnhub.io/api/v1/quote" } };
	auto params{
		cpr::Parameters{
			{ "symbol", symbol }
		}
	};

	return ensured_finnhub_api_request(url, params);
}
