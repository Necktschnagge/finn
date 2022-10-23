#include "logger.h"

#include "secrets.h"

#include <cpr/cpr.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>

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

static const std::string candles_Nvidia{ "candles_Nvidia" };


nlohmann::json get_candles(const std::string& symbol) {
	std::cout << "\nget all candles...";

	//https://finnhub.io/api/v1/stock/candle?symbol=AAPL&resolution=1&from=1631022248&to=1631627048
	cpr::Response r = cpr::Get(cpr::Url{ "https://finnhub.io/api/v1/stock/candle" },
		//cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
		cpr::Parameters{
			{"symbol", symbol},
		{"resolution", "1"},
		{"from", "1656662061"},
		{"to", "1664531661"},
		{"token", secret_token.data()}
		});

	nlohmann::json candles = nlohmann::json::parse(r.text);

	//std::cout << "\ncheck list..." << stock_list.is_array();
	//std::cout << "\ncheck object..." << stock_list.is_object();

	return candles;
}


nlohmann::json get_share_details(const std::string& symbol) {
	std::cout << "\nget details...";
	//https://finnhub.io/api/v1/quote?symbol=AAPL

	cpr::Response r = cpr::Get(cpr::Url{ "https://finnhub.io/api/v1/quote" },
		//cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
		cpr::Parameters{ {"symbol", symbol}, {"token", secret_token.data()} });
	/*
	std::cout << r.status_code << '\n'                // 200
		<< r.header["content-type"] << '\n'       // application/json; charset=utf-8
		<< r.text;                         // JSON text string
	*/

	nlohmann::json details = nlohmann::json::parse(r.text);

	//std::cout << "\ncheck list..." << details.is_array();
	//std::cout << "\ncheck object..." << details.is_object();

	return details;
}


int main()
{
	init_logger();
	const std::string json_file_name{ "summary.json" };
	auto finn{ finnhub_rest_client(secret_token.data()) };

	nlohmann::json summary{ nlohmann::json::object() };
	//load json
	std::ifstream ofile;
	ofile.open(json_file_name);
	if (ofile.good()) { // check if opened a file
		ofile >> std::noskipws;
		try {
			summary = nlohmann::json::parse(std::istream_iterator<std::ifstream::char_type>(ofile), std::istream_iterator<std::ifstream::char_type>());
		}
		catch (...) {
			std::cerr << "Could not parse opened file named:   " << json_file_name << std::endl;
		}
		ofile.close();
	}
	else {
		std::cerr << "Could not open a file named:   " << json_file_name << std::endl;
	}

	// get all US stock symbols...
	if (true) {
		auto stock_list = finn.getStockSymbols("US");
		const std::string key{ "stock_list_US" };

		summary[key] = stock_list;
		if (summary[key].is_array())
			std::sort(
				summary[key].begin(),
				summary[key].end(),
				[](const auto& a, const auto& b) {
					try {
						return a.at("symbol") < b.at("symbol");
					}
					catch (...) {
						return false;
					}
				}
		);
	}

	// get all DE stock symbols...
	if (true) {
		auto stock_list = get_stock_symbols("DE");
		const std::string key{ "stock_list_DE" };

		summary[key] = stock_list;
		if (summary[key].is_array())
			std::sort(
				summary[key].begin(),
				summary[key].end(),
				[](const auto& a, const auto& b) {
					try {
						return a.at("symbol") < b.at("symbol");
					}
					catch (...) {
						return false;
					}
				}
		);

	}

	// get candles for NVDA...
	if (true) {
		auto candle_chart_data = get_candles("NVDA");

		summary[candles_Nvidia] = candle_chart_data;
	}

	// get details of a single stock

	if (true) {
		summary["details_Nvidia"] = get_share_details("NVDA");
		summary["details_Intel"] = get_share_details("INTC");
	}

	//save json
	std::ofstream file;
	file.open(json_file_name);
	file << summary.dump(3);

	return 0;

}

#if false
//////////////// a bunch of trash:

if (false) { // rename a key...
	summary["stock_list_US"] = summary["stock_list"];
	auto it = summary.find("stock_list");
	auto jt = it;
	summary.erase("stock_list");
}

#endif

