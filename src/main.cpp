#include "logger.h"

#include <cpr/cpr.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>



nlohmann::json get_stock_symbols(const std::string& exchange) {
	std::cout << "\nget all symbols...";
	//https://finnhub.io/api/v1/stock/symbol?exchange=US&token=caeqfm2ad3i9ra0rgja0
	cpr::Response r = cpr::Get(cpr::Url{ "https://finnhub.io/api/v1/stock/symbol" },
		//cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
		cpr::Parameters{ {"exchange", exchange}, {"token", secret_token} });
	/*
	std::cout << r.status_code << '\n'                // 200
		<< r.header["content-type"] << '\n'       // application/json; charset=utf-8
		<< r.text;                         // JSON text string
	*/

	nlohmann::json stock_list = nlohmann::json::parse(r.text);

	std::cout << "\ncheck list..." << stock_list.is_array();
	std::cout << "\ncheck object..." << stock_list.is_object();

	return stock_list;
}


nlohmann::json get_candles(const std::string& symbol) {
	std::cout << "\nget all symbols...";

	//https://finnhub.io/api/v1/stock/candle?symbol=AAPL&resolution=1&from=1631022248&to=1631627048
	cpr::Response r = cpr::Get(cpr::Url{ "https://finnhub.io/api/v1/stock/candle" },
		//cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
		cpr::Parameters{
			{"symbol", symbol},
		{"resolution", "1"},
		{"from", "1656662061"},
		{"to", "1664531661"},
		{"token", secret_token}
		});

	nlohmann::json stock_list = nlohmann::json::parse(r.text);

	std::cout << "\ncheck list..." << stock_list.is_array();
	std::cout << "\ncheck object..." << stock_list.is_object();

	return stock_list;
}


nlohmann::json get_share_details(const std::string& symbol) {


	std::cout << "\nget details...";
	//https://finnhub.io/api/v1/quote?symbol=AAPL

	cpr::Response r = cpr::Get(cpr::Url{ "https://finnhub.io/api/v1/quote" },
		//cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
		cpr::Parameters{ {"symbol", symbol}, {"token", secret_token} });
	/*
	std::cout << r.status_code << '\n'                // 200
		<< r.header["content-type"] << '\n'       // application/json; charset=utf-8
		<< r.text;                         // JSON text string
	*/

	nlohmann::json details = nlohmann::json::parse(r.text);

	std::cout << "\ncheck list..." << details.is_array();
	std::cout << "\ncheck object..." << details.is_object();

	return details;
}


int main()
{
	init_logger();
	const std::string json_file_name{ "summary.json" };

	nlohmann::json summary;
	//load json
	std::ifstream ofile;
	ofile.open(json_file_name);
	ofile >> std::noskipws;
	summary = nlohmann::json::parse(std::istream_iterator<std::ifstream::char_type>(ofile), std::istream_iterator<std::ifstream::char_type>());
	ofile.close();

	// get all US stock symbols...
	if (false) {
		auto stock_list = get_stock_symbols("US");
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
	if (false) {
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

	// get candles...
	if (true) {
		auto stock_list = get_candles("NVDA");
		const std::string key{ "candles_Nvidia" };

		summary[key] = stock_list;
		if (false)
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

	// get details of a single stock

	if (false) {
		summary["details_Nvidia"] = get_share_details("NVDA");
		summary["details_Intel"] = get_share_details("INTC");
	}

	if (false) { // rename a key...
		summary["stock_list_US"] = summary["stock_list"];
		auto it = summary.find("stock_list");
		auto jt = it;
		summary.erase("stock_list");
	}

	//save json
	std::ofstream file;
	file.open(json_file_name);
	file << summary.dump(3);

	return 0;

}

