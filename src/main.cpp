#include "logger.h"

#include "utility.h"
#include "secrets.h"
#include "finnhub_rest_client.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>

namespace playground {
	static const std::string json_file_name{ "summary.json" };

	static const std::string DE{ "DE" };
	static const std::string US{ "US" };

	namespace sheep {

		static const std::string stock_list_DE{ "stock_list_DE" };
		static const std::string stock_list_US{ "stock_list_US" };

		
		static const std::string NVDA{ "NVDA" };
		static const std::string NVDA_Quotes{ "NVDA-Quote" };
		static const std::string NVDA_Candles{ "NVDA-Candles" };


		static const std::string INTC{ "INTC" };
		static const std::string INTC_Quotes{ "INTC-Quote" };
		static const std::string INTC_Candles{ "INTC-Candles" };

	}

	namespace rooster {

	}

}


int main()
{
	init_logger();
	
	auto finn{ finnhub_rest_client(secret_token.data()) };
	
	nlohmann::json summary{ load_json(playground::json_file_name) };
	
	// get all US stock symbols...
	if (true) {
		auto stock_list = finn.getStockSymbols(playground::US);
		summary[playground::sheep::stock_list_US] = stock_list;
		try_sort_stock_list(summary[playground::sheep::stock_list_US]);
	}

	// get all DE stock symbols...
	if (true) {
		auto stock_list = finn.getStockSymbols(playground::DE);
		summary[playground::sheep::stock_list_DE] = stock_list;
		try_sort_stock_list(summary[playground::sheep::stock_list_DE]);
	}

	// get candles for...
	if (true) {
		summary[playground::sheep::NVDA_Candles] = finn.getStockCandles(playground::sheep::NVDA, 1656662061, 1664531661, 1);
		summary[playground::sheep::INTC_Candles] = finn.getStockCandles(playground::sheep::INTC, 1656662061, 1664531661, 1);
		// TODO: list all possible resolutions!
	}

	// get curreent price, delta, open, previous close, day high, day low... of a single stock
	if (true) {
		summary[playground::sheep::NVDA_Quotes] = finn.getQuotes(playground::sheep::NVDA);
		summary[playground::sheep::INTC_Quotes] = finn.getQuotes(playground::sheep::INTC);
	}

	//save json
	save_json(playground::json_file_name, summary);
	return 0;
}
