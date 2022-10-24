#include "logger.h"

#include "utility.h"
#include "secrets.h"
#include "finnhub_rest_client.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>

static const std::string candles_Nvidia{ "candles_Nvidia" };
static const std::string json_file_name{ "summary.json" };


int main()
{
	init_logger();
	
	auto finn{ finnhub_rest_client(secret_token.data()) };
	
	nlohmann::json summary{ load_json(json_file_name) };
	
	// get all US stock symbols...
	if (true) {
		auto stock_list = finn.getStockSymbols("US");
		const std::string key{ "stock_list_US" };
		summary[key] = stock_list;
		try_sort_stock_list(summary[key]);
	}

	// get all DE stock symbols...
	if (true) {
		auto stock_list = finn.getStockSymbols("DE");
		const std::string key{ "stock_list_DE" };
		summary[key] = stock_list;
		try_sort_stock_list(summary[key]);
	}

	// get candles for NVDA...
	if (true) {
		summary[candles_Nvidia] = finn.getStockCandles("NVDA", 1656662061, 1664531661, 1);
		// TODO: list all possible resolutions!
	}

	// get details of a single stock

	if (true) {
		summary["details_Nvidia"] = finn.getQuotes("NVDA");
		summary["details_Intel"] = finn.getQuotes("INTC");
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

