#include "logger.h"

#include "secrets.h"

#include "utility.h"
#include "finnhub_rest_client.h"

#include <nlohmann/json.hpp>


namespace feature_toogle {

	namespace sheep {
		static constexpr bool FOLD_US_STOCK_CURRENCIES{ false };
		static constexpr bool TEST_API_LIMIT{ false };
		static bool GET_ALL_SHARES_DETAILS{ false };
	}
}

namespace fnn {
	static const std::string symbol{ "symbol" };
	static const std::string Profile2{ "Profile2" };
	static const std::string Basics{ "Basics" };


}

namespace playground {
	static const std::string json_file_name{ "summary.json" };

	static const std::string DE{ "DE" };
	static const std::string US{ "US" };

	namespace sheep {

		static const std::string stock_list_DE{ "stock_list_DE" };
		static const std::string stock_list_US{ "stock_list_US" };

		static const std::string stock_list_US_currencies{ "stock_list_US_currencies" };

		static const std::string News{ "News" };

		static const std::string AMD{ "AMD" };
		static const std::string AMD_ISIN{ "US0079031078" };

		static const std::string BBBY{ "BBBY" };
		static const std::string BBBY_Quotes{ "BBBY-Quote" };
		static const std::string BBBY_Candles{ "BBBY-Candles" };
		static const std::string BBBY_Profile2{ "BBBY-Profile2" };


		static const std::string NVDA{ "NVDA" };
		static const std::string NVDA_Quotes{ "NVDA-Quote" };
		static const std::string NVDA_Candles{ "NVDA-Candles" };
		static const std::string NVDA_Basics{ "NVDA-Basics" };
		static const std::string NVDA_Profile2{ "NVDA-Profile2" };


		static const std::string INTC{ "INTC" };
		static const std::string INTC_Quotes{ "INTC-Quote" };
		static const std::string INTC_Candles{ "INTC-Candles" };
		static const std::string INTC_Profile2{ "INTC-Profile2" };
	}

}

void finnhub_example_api_requests(finnhub_rest_client& finn, nlohmann::json& summary) {
	// get all US stock symbols...
	if (true) {
		auto stock_list = finn.getStockSymbol(playground::US);
		summary[playground::sheep::stock_list_US] = stock_list;
		try_sort_stock_list(summary[playground::sheep::stock_list_US]);
	}

	// get all DE stock symbols... needs expensive purchase plan...
	if (false) {
		auto stock_list = finn.getStockSymbol(playground::DE);
		summary[playground::sheep::stock_list_DE] = stock_list;
		try_sort_stock_list(summary[playground::sheep::stock_list_DE]);
	}

	// get candles for...
	if (true) {
		summary[playground::sheep::NVDA_Candles] = finn.getStockCandles(playground::sheep::NVDA, 1656662061, 1664531661, 1);
		//summary[playground::sheep::INTC_Candles] = finn.getStockCandles(playground::sheep::INTC, 1656662061, 1664531661, 1);
		// TODO: list all possible resolutions!
	}

	// get current price, delta, open, previous close, day high, day low... of a single stock
	if (true) {
		summary[playground::sheep::NVDA_Quotes] = finn.getQuote(playground::sheep::NVDA);
		//summary[playground::sheep::INTC_Quotes] = finn.getQuote(playground::sheep::INTC);
		//summary[playground::sheep::BBBY_Quotes] = finn.getQuote(playground::sheep::BBBY);
	}

	// get company profile...
	if (true) {
		summary[playground::sheep::NVDA_Profile2] = finn.getCompanyProfile2(playground::sheep::NVDA);
		//summary[playground::sheep::INTC_Profile2] = finn.getCompanyProfile2(playground::sheep::INTC);
		//summary[playground::sheep::BBBY_Profile2] = finn.getCompanyProfile2(playground::sheep::BBBY);


		summary[playground::sheep::NVDA_Basics] = finn.getBasicFinancials(playground::sheep::NVDA);
	}

	// search for a share using ISIN
	if (true) {
		summary[playground::sheep::AMD] = finn.getSymbolLookup(playground::sheep::AMD_ISIN);
	}

	// get general market news
	if (true) {
		summary[playground::sheep::News] = finn.getNews(finnhub_rest_client::market_news_category::values::general);
	}

	// check which currencies US stocks can have...
	if constexpr (feature_toogle::sheep::FOLD_US_STOCK_CURRENCIES) {
		summary[playground::sheep::stock_list_US_currencies] = fold_json_object_array_into_value_set(summary[playground::sheep::stock_list_US], "currency");
	}

	// check if the application still works fine when exceeding the api limit...
	if constexpr (feature_toogle::sheep::TEST_API_LIMIT) {
		using namespace std::chrono_literals;
		for (int x = 0; x < 70; ++x) {

			nlohmann::json test = finn.getQuote(playground::sheep::NVDA);
			std::this_thread::sleep_for(200ms);
		}
	}


}


int main(int argc, char* argv[])
{
	init_logger();

	if (argc > 1) {
		//on server
		feature_toogle::sheep::GET_ALL_SHARES_DETAILS = false;
		(void)argv;
	}

	auto finn{ finnhub_rest_client(secret_token.data()) };

	// load previous json...
	nlohmann::json summary = load_json(playground::json_file_name);

	finnhub_example_api_requests(finn, summary);

	//save json
	save_json(playground::json_file_name, summary);
	return 0;
}
