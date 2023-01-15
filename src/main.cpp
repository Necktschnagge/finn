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
constexpr auto SECONDS_PER_DAY() -> decltype(
	std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(24)).count()
	) {
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(24)).count();
}

class candle {
public:
	double open;
	double close;
	double high;
	double low;
	double volume;

	uint64_t time;
};

void finnhub_example_api_requests(finnhub_rest_client& finn, nlohmann::json& summary) {

	const auto now_seconds{ std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() };

	const auto today_at_0{
		now_seconds / SECONDS_PER_DAY() * SECONDS_PER_DAY()
	};
	(void)today_at_0;
	const auto p1 = std::chrono::system_clock::now();

	standard_logger()->error(now_seconds % SECONDS_PER_DAY() / 60 / 60.0); // system clock indeed is utc clock.

	std::stringstream ss;
	ss << "seconds since epoch: "
		<< std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()).count() << '\n';
	standard_logger()->info(ss.str());

	std::vector<candle> candles;

	for (int64_t i = -360; i != 0; ++i) {
		// stock exchange closing time is 1:00 utc . So let's begin the day at 2:00!
		const auto start = today_at_0 + i * SECONDS_PER_DAY() + (2 * 60 * 60);
		const auto stop = today_at_0 + (i + 1) * SECONDS_PER_DAY() + (2 * 60 * 60);
		summary["NVDA-TEST"][std::to_string(start)] = finn.getStockCandles(playground::sheep::NVDA, start, stop, 1);

		if (summary["NVDA-TEST"][std::to_string(start)]["s"].get<std::string>() == "ok") {
			auto i_open = summary["NVDA-TEST"][std::to_string(start)]["o"].cbegin();
			auto i_close = summary["NVDA-TEST"][std::to_string(start)]["c"].cbegin();
			auto i_high = summary["NVDA-TEST"][std::to_string(start)]["h"].cbegin();
			auto i_low = summary["NVDA-TEST"][std::to_string(start)]["l"].cbegin();
			auto i_time = summary["NVDA-TEST"][std::to_string(start)]["t"].cbegin();
			auto i_volume = summary["NVDA-TEST"][std::to_string(start)]["v"].cbegin();

			const auto end_open = summary["NVDA-TEST"][std::to_string(start)]["o"].cend();
			const auto end_close = summary["NVDA-TEST"][std::to_string(start)]["c"].cend();
			const auto end_high = summary["NVDA-TEST"][std::to_string(start)]["h"].cend();
			const auto end_low = summary["NVDA-TEST"][std::to_string(start)]["l"].cend();
			const auto end_time = summary["NVDA-TEST"][std::to_string(start)]["t"].cend();
			const auto end_volume = summary["NVDA-TEST"][std::to_string(start)]["v"].cend();

			for (; i_open != end_open && i_close != end_close && i_high != end_high && i_low != end_low && i_time != end_time && i_volume != end_volume;) {
				candles.emplace_back();
				candles.back().open = i_open->get<double>();
				candles.back().close = i_close->get<double>();
				candles.back().high = i_high->get<double>();
				candles.back().low = i_low->get<double>();
				candles.back().time = i_time->get<uint64_t>();
				candles.back().volume = i_volume->get<double>();

				++i_open;
				++i_close;
				++i_high;
				++i_low;
				++i_time;
				++i_volume;
			}
		}

	}

	std::sort(candles.begin(), candles.end(), [](const candle& a, const candle& b) { return a.time < b.time; });

	std::map<int64_t, uint64_t> time_gaps; // time gap |-> count who often happened
	
	for (auto iter = candles.cbegin(); iter != candles.cend() && std::next(iter) != candles.cend(); ++iter) {
		const auto jter = std::next(iter);
		++time_gaps[jter->time - iter->time];
	}
	for (auto pair : time_gaps) {
		summary["gaps"][std::to_string(pair.first)] = pair.second;
		standard_logger()->info(std::string("got") + std::to_string(pair.first) + ":" + std::to_string(pair.second));
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
