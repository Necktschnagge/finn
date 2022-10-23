#include "logger.h"

#include "secrets.h"
#include "finnhub_rest_client.h"

#include <cpr/cpr.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>

class finnhub_rest_client2;

static const std::string candles_Nvidia{ "candles_Nvidia" };


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
		auto stock_list = finn.getStockSymbols("DE");
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
		summary[candles_Nvidia] = finn.getStockCandles("NVDA", 1656662061, 1664531661, 1);
		// TODO: list all possible resolutions!
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

