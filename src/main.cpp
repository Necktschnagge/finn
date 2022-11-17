#include "logger.h"

#include "utility.h"
#include "secrets.h"
#include "finnhub_rest_client.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <chrono>

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

		static const std::string map_stocks_to_details{ "map_stocks_to_details" };
		static const std::string finnhub_client_meta{ "finnhub_client_meta" };
		static const std::string http_status_codes{ "http_status_codes" };
		static const std::string original{ "original" };


	}

	namespace rooster {

	}

}


void split_big_json_and_save() {
	// load previous json...
	standard_logger()->info("start parsing json");
	nlohmann::json summary = load_json(playground::json_file_name);
	standard_logger()->info("stop parsing json");

	std::array<nlohmann::json, 26> splitted;

	std::for_each(
		summary[playground::sheep::stock_list_US].cbegin(),
		summary[playground::sheep::stock_list_US].cend(),
		[&splitted, &summary](const nlohmann::json& item) {
			std::string sym{ item[fnn::symbol] };
			std::string first_letter = sym.substr(0, 1);
			if (first_letter.empty()) first_letter = "A";
			first_letter[0] = std::toupper(first_letter[0], std::locale());
			uint8_t index = (first_letter[0] - 'A');
			index = index > 25 ? 0 : index;
			splitted[index][sym] = summary[playground::sheep::map_stocks_to_details][sym];
		});

	for (uint8_t i = 0; i < 26; ++i) {
		std::string name{ std::to_string('A' + i) + ".json" };
		save_json(name, splitted[i]);
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

	//split_big_json_and_save();


	standard_logger()->info("start parsing jsons");
	std::array<nlohmann::json, 26> splitted;
	std::array<std::thread, 26> the_pool;
	for (uint8_t i = 0; i < 26; ++i) {
		std::string name{ std::to_string('A' + i) + ".json" };
		the_pool[i] = std::thread(
			[&splitted](std::string name, uint8_t i) {
				splitted[i] = load_json(name);
			},
			name,
			i
		);
		
	}
	for (uint8_t i = 0; i < 26; ++i) {
		the_pool[i].join();
	}
	standard_logger()->info("stop parsing jsons");
	standard_logger()->info("stop");

#if false

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
	if (false) {
		summary[playground::sheep::AMD] = finn.getSymbolLookup(playground::sheep::AMD_ISIN);
	}

	// get general market news
	if (false) {
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

	/*

	ideas for filtering shares:

	Profile2
		market cap: marketCapitalization in Mio USD
		number of distributed shares: shareOutstanding (unit: Mio Shares)
		ipo date:

	Basics
		metric:
			10DayAverageTradingVolume: (10 day average number of shares traded per day, unit: Mio. Shares)

	*/

	//nlohmann::json selected_shares = nlohmann::json::array();

	// Profile2 for each share
	if (feature_toogle::sheep::GET_ALL_SHARES_DETAILS) {
		std::size_t count_shares{ summary[playground::sheep::stock_list_US].size() };
		standard_logger()->info("Number of shares:");
		standard_logger()->info(count_shares);
		standard_logger()->info("Minutes needed:");
		standard_logger()->info(count_shares / 60);

		std::for_each(
			summary[playground::sheep::stock_list_US].cbegin(),
			summary[playground::sheep::stock_list_US].cend(),
			//std::next(summary[playground::sheep::stock_list_US].cbegin(), 40),
			[&finn, &summary](const nlohmann::json& item) {
				std::string sym{ item[fnn::symbol] };
				uint64_t s1 = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				summary[playground::sheep::map_stocks_to_details][sym][fnn::Profile2][std::to_string(s1)] = finn.getCompanyProfile2(sym);
				uint64_t s2 = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				summary[playground::sheep::map_stocks_to_details][sym][fnn::Basics][std::to_string(s2)] = finn.getBasicFinancials(sym);
			});

		try {
			const auto& original_status_codes{ finn.export_meta_status_codes() };
			std::map<uint16_t, uint64_t> count_codes;
			for (auto iter = original_status_codes.cbegin(); iter != original_status_codes.cend(); ++iter) {
				++count_codes[*iter];
			}

			summary[playground::sheep::finnhub_client_meta][playground::sheep::http_status_codes][playground::sheep::original] = original_status_codes;
			for (const auto& pair : count_codes) {
				summary[playground::sheep::finnhub_client_meta][playground::sheep::http_status_codes][std::to_string(pair.first)] = pair.second;
			}
		}
		catch (const std::runtime_error& e)
		{
			standard_logger()->error("Cannot export response code meta data:");
			standard_logger()->error(e.what());
		}
	}


	//save json
	//save_json(playground::json_file_name, summary);
	return 0;
#endif
}





#if false

summary[playground::sheep::map_stocks_to_details];
std::back_inserter(selected_shares),
#endif