#include "logger.h"

#include "secrets.h"

#include "utility.h"
#include "finnhub_rest_client.h"

#include <nlohmann/json.hpp>

#include <sstream>
#include <iomanip>

template <class T>
std::string sized_to_string(const T& value, const std::streamsize& width) {
	std::stringstream ss;
	ss << std::setw(width) << std::setfill('0') << value;
	return ss.str();
}

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

class bordered_candle {
public:
	candle c;
	uint64_t first_time;
	uint64_t last_time;
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
		if constexpr (false) summary["NVDA-TEST"][std::to_string(start)] = finn.getStockCandles(playground::sheep::NVDA, start, stop, 1);

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
	for (const auto& pair : time_gaps) {
		summary["gaps"][sized_to_string(pair.first, 7)] = pair.second;
		standard_logger()->info(std::string("got  ") + std::to_string(pair.first) + "  \t:  " + std::to_string(pair.second));
	}

	std::vector<bordered_candle> candles_3_h;

	uint64_t reference_time = 0;
	for (const auto& element : candles) {
		const uint64_t _3_hours{ 3 * 60 * 60 };

		if (reference_time == 0 || (element.time - reference_time > _3_hours)) { // no ref time or at least 3 hours gone...
			if (reference_time != 0) { // leave old bordered_candle
				// nothing to do.
			}
			candles_3_h.emplace_back(); // insert new bordered_candle, init:
			candles_3_h.back().first_time = element.time;
			candles_3_h.back().last_time = element.time;
			candles_3_h.back().c = element;
			candles_3_h.back().c.time = 0;
			candles_3_h.back().c.volume = 0;
			
			reference_time = (element.time / _3_hours) * _3_hours;
		}
		
		// add element to current bordered_candle
		candles_3_h.back().first_time = std::min(candles_3_h.back().first_time, element.time); // logically useless
		candles_3_h.back().last_time = std::max(candles_3_h.back().last_time, element.time);
		candles_3_h.back().c.high = std::max(candles_3_h.back().c.high, element.high);
		candles_3_h.back().c.low = std::min(candles_3_h.back().c.low, element.low);
		candles_3_h.back().c.close = element.close;
		candles_3_h.back().c.volume += element.volume;

	}

	standard_logger()->info("done.");

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
