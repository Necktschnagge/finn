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
	static const std::string marketCapitalization{ "marketCapitalization" };


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

namespace fsl {

	class isJsonObject {
		template<class JsonView>
		// the view which is already given as basis...
		static bool check(const JsonView& json) noexcept {
			return json.underlying.is_object();
		}
	};

	class isJsonArray {
		template<class JsonView>
		static bool check(const JsonView& json) noexcept {
			return json.underlying.is_array();
		}
	};

	template<std::string_view* key>
	class hasObjectProperty {
		template<class JsonView>
		static bool check(const JsonView& json) noexcept {
			json.check<isJsonObject>(); // check dependencies
			// check already required dependent properties.
			// check the actual property
			return true;
		}
	};

	class strategy {

	};

	strategy check; // assume, check, filter, antifilter
	strategy assume; // assume, check, filter, antifilter
	strategy filter; // assume, check, filter, antifilter
	strategy antifilter; // assume, check, filter, antifilter



	// view checks properties.
	// view knows about all properties which were alreeady checked.
	/*
	view <A,B,C,D> := view_add<A>(view<B...D>)
	view.check_prop<Prop>()

	*/
	template<class ... Properties>
	class attributed_json_view {
	public:
		using json_type = nlohmann::json;
		json_type& json;

		class strategy {

		};

		strategy check; // assume, check, filter, antifilter

		attributed_json_view(json_type& json, const strategy& s = check) : json(json) {
			//Properties...::check();

		}

	};

	template<class the_property>
	class simple_view {
	public:
		using json_type = nlohmann::json;

		json_type& json;

		class broken_view : public std::runtime_error {

		};

		attributed_json_view(json_type& json, const strategy& s = check) : json(json) {
			if (s == assume)
				return;
			bool check_result = the_property.check(*this);
			if (s == check) {
				if (!check_result)
					throw broken_view("");
			}
		}

		json_type& underlying() {
			return json;
		}



	};

	class broken_view : public std::runtime_error {

	};

	template<class JsonType>
	class true_view {
	public:
		using json_type = JsonType;

	protected:
		json_type& json;

	public:
		true_view(json_type& json, const strategy& s = check) : json(json) {
			(void)s; // ignore the strategy
		}

		json_type& underlying() {
			return json;
		}

		template<class Property>
		bool already_checked(const strategy& s) {
			return false;
		}
	};

	template<class the_property, class the_basic_view>
	class hierachy_view : public the_basic_view {
	public:
		using json_type = nlohmann::json;

		the_basic_view& basic_view() {
			return static_cast<the_basic_view&>(*this);
		}

		hierachy_view(json_type& json/*, const strategy& s = check*/) : the_basic_view(json/*, s*/) {
			//if (s == assume)
			//	return;
			bool check_result = the_property.check(basic_view());
			//if (s == check) {
			if (!check_result)
				throw broken_view("");
			//}
		}

		json_type& underlying() {
			return json;
		}

		template<class Property>
		constexpr bool already_checked(/*const strategy& s*/) {
			// check if given property already asserted within this hierachy.
			// (assuming in previous steps all properties of this hierarchy had been checked)
			if constexpr (std::is_same<Property, the_property>::value) {
				return true;
			}
			return the_basic_view::already_checked(/*s*/);
		}

	};

}


[[deprecated]] void split_big_json_and_save1() {
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

void split_big_json_and_save(const std::size_t& SPLITTING_FACTOR) {

	// load previous json...
	standard_logger()->info("start parsing json");
	nlohmann::json summary = load_json(playground::json_file_name);
	standard_logger()->info("start splitting json");

	auto splitted{ std::vector<nlohmann::json>(SPLITTING_FACTOR) };

	const std::size_t SOCK_LIST_SIZE{ summary[playground::sheep::stock_list_US].size() };

	std::size_t i{ 0 };
	for (auto iter = summary[playground::sheep::stock_list_US].cbegin();
		iter != summary[playground::sheep::stock_list_US].cend();
		++iter, ++i) {
		std::string sym{ (*iter)[fnn::symbol] };
		const std::size_t index{ i * SPLITTING_FACTOR / SOCK_LIST_SIZE };

		splitted[index][sym] = summary[playground::sheep::map_stocks_to_details][sym];
	}

	standard_logger()->info("start saving splitted jsons");
	for (uint8_t j = 0; j < SPLITTING_FACTOR; ++j) {
		std::string name{ std::string("share-details-") + std::to_string(j) + ".json" };
		save_json(name, splitted[j]);
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

	constexpr std::size_t SPLITTING_FACTOR{ 16 };

	//split_big_json_and_save();
	//split_big_json_and_save(SPLITTING_FACTOR);


	standard_logger()->info("start parsing splitted jsons");
	std::array<nlohmann::json, SPLITTING_FACTOR> splitted;
	std::array<std::thread, SPLITTING_FACTOR> the_pool;
	for (uint8_t i = 0; i < SPLITTING_FACTOR; ++i) {
		std::string name{ std::string("share-details-") + std::to_string(i) + ".json" };
		the_pool[i] = std::thread(
			[&splitted](std::string name, uint8_t i) {
				splitted[i] = load_json(name);
			},
			name,
				i
				);

	}
	for (auto& t : the_pool) {
		t.join();
	}
	standard_logger()->info("ready parsing splitted jsons");
	/*
	standard_logger()->info("start joining splitted jsons");
	for (uint8_t i = 1; i < SPLITTING_FACTOR; ++i) {
		splitted[0].update(splitted[i]);
	}
	standard_logger()->info("ready joining splitted jsons");
	*/
	const auto hasAnyTimeStampKey{ // requires isObject (?)
		[](const nlohmann::json& j) {
			for (auto iter = j.cbegin(); iter != j.cend(); ++iter) {
				try {
					uint64_t converted = std::stoull(iter.key());
					if (j.find(std::to_string(converted)) != j.cend()) {
						return true;
					}
				}
				catch (...) {} // ignore exceptions thrown by std::stoull, .key()
			}
		return false;
		}
	};


	// check for a timestamp
	for (uint8_t i = 0; i < SPLITTING_FACTOR; ++i) {
		for (auto iter = splitted[i].cbegin(); iter != splitted[i].cend(); ++iter) {
			if (!hasAnyTimeStampKey(iter.value()[fnn::Basics]) || !hasAnyTimeStampKey(iter.value()[fnn::Profile2])) {
				standard_logger()->error("unexpected json value");
			}
		}
	}

	const auto getLatestTimeStampKey{ // requires hasAnyTimeStampKey
	[](const nlohmann::json& j) -> uint64_t {
		std::vector<uint64_t> time_stamps;
		for (auto iter = j.cbegin(); iter != j.cend(); ++iter) {
			try {
				uint64_t converted = std::stoull(iter.key());
				if (j.find(std::to_string(converted)) != j.cend()) {
					time_stamps.push_back(converted);
				}
			}
			catch (...) {} // ignore exceptions thrown by std::stoull, .key()
		}
		std::sort(time_stamps.cbegin(), time_stamps.cend());
		return time_stamps.back();
	}
	};

	const auto getMarketCapitalizationofProfile2{
		[](const nlohmann::json& j) -> double {
			if (!j[fnn::marketCapitalization].is_number())
				return 0;
			try {
				j[fnn::marketCapitalization]

				}
			}
			catch (...) {} // ignore exceptions thrown by std::stoull, .key()
		std::sort(time_stamps.cbegin(), time_stamps.cend());
		return time_stamps.back();
	}
};

// sharesOutstandin,
//	Marketcap,
//-> trading volume






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