#include "logger.h"

#include "utility.h"
#include "secrets.h"
#include "finnhub_rest_client.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>

namespace feature_toogle {

	namespace sheep {
		static constexpr bool FOLD_US_STOCK_CURRENCIES{ true };
	}
}

namespace playground {
	static const std::string json_file_name{ "summary.json" };

	static const std::string DE{ "DE" };
	static const std::string US{ "US" };

	namespace sheep {

		static const std::string stock_list_DE{ "stock_list_DE" };
		static const std::string stock_list_US{ "stock_list_US" };

		static const std::string stock_list_US_currencies{ "stock_list_US_currencies" };


		static const std::string NVDA{ "NVDA" };
		static const std::string NVDA_Quotes{ "NVDA-Quote" };
		static const std::string NVDA_Candles{ "NVDA-Candles" };
		static const std::string NVDA_Profile2{ "NVDA-Profile2" };


		static const std::string INTC{ "INTC" };
		static const std::string INTC_Quotes{ "INTC-Quote" };
		static const std::string INTC_Candles{ "INTC-Candles" };
		static const std::string INTC_Profile2{ "INTC-Profile2" };

	}

	namespace rooster {

	}

}


int main()
{
	init_logger();

	auto finn{ finnhub_rest_client(secret_token.data()) };

	// load previous json...
	nlohmann::json summary{ load_json(playground::json_file_name) };

	// get all US stock symbols...
	if (true) {
		auto stock_list = finn.getStockSymbols(playground::US);
		summary[playground::sheep::stock_list_US] = stock_list;
		try_sort_stock_list(summary[playground::sheep::stock_list_US]);
	}

	// get all DE stock symbols... needs expensive purchase plan...
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

	// get current price, delta, open, previous close, day high, day low... of a single stock
	if (true) {
		summary[playground::sheep::NVDA_Quotes] = finn.getQuotes(playground::sheep::NVDA);
		summary[playground::sheep::INTC_Quotes] = finn.getQuotes(playground::sheep::INTC);
	}

	// get company profile...
	if (true) {
		summary[playground::sheep::NVDA_Profile2] = finn.getStockProfile2(playground::sheep::NVDA);
		summary[playground::sheep::INTC_Profile2] = finn.getStockProfile2(playground::sheep::INTC);
	}

	if constexpr (feature_toogle::sheep::FOLD_US_STOCK_CURRENCIES) {
		std::set<std::string> currencies;
		if (!summary[playground::sheep::stock_list_US].is_array()) {
			standard_logger()->error("Fold currencies type error 1");
		}
		else {
			std::transform(
				summary[playground::sheep::stock_list_US].cbegin(),
				summary[playground::sheep::stock_list_US].cend(),
				std::inserter(currencies, currencies.cbegin()),
				[](const nlohmann::json& obj) {
					try {
						return obj.at("currency").get<std::string>();
					}
					catch (...) {
						return std::string("ERROR");
					}
				}
			);
		}
		summary[playground::sheep::stock_list_US_currencies] = nlohmann::json::array();
		std::copy(currencies.cbegin(), currencies.cend(), std::back_inserter(summary[playground::sheep::stock_list_US_currencies]));
	}

	//save json
	save_json(playground::json_file_name, summary);
	return 0;
}
