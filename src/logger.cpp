#include "logger.h"

#include "internal_error.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"

#include <iostream>


namespace {
	
	const std::string STANDARD_LOGGER_NAME{ "main" };
	const std::string FINNHUB_CLIENT_LOGGER_NAME{ "finnhub-client" };
	const std::string UTILITY_LOGGER_NAME{ "utility" };

}


std::shared_ptr<spdlog::logger> standard_logger(){
	auto s_ptr = spdlog::get(STANDARD_LOGGER_NAME);
	fsl::internal_error::assert_true(s_ptr.operator bool(), "The standard logger has not been registered.");
	return s_ptr;
	// when returning raw pointer a warning is logged since the shared_ptr will be destroyed, but the underlying pointer will be retained
}

std::shared_ptr<spdlog::logger> finnhub_client_logger(){
	auto s_ptr = spdlog::get(FINNHUB_CLIENT_LOGGER_NAME);
	fsl::internal_error::assert_true(s_ptr.operator bool(), "The finnhub client logger has not been registered.");
	return s_ptr;
	// when returning raw pointer a warning is logged since the shared_ptr will be destroyed, but the underlying pointer will be retained
}

std::shared_ptr<spdlog::logger> utility_logger(){
	auto s_ptr = spdlog::get(UTILITY_LOGGER_NAME);
	fsl::internal_error::assert_true(s_ptr.operator bool(), "The utility logger has not been registered.");
	return s_ptr;
	// when returning raw pointer a warning is logged since the shared_ptr will be destroyed, but the underlying pointer will be retained
}

void init_logger(){
	auto sink_std_cout = std::make_shared<spdlog::sinks::ostream_sink_mt>(std::cout);

	auto standard_logger = std::make_shared<spdlog::logger>(STANDARD_LOGGER_NAME, sink_std_cout);
	auto finnhub_client_logger = std::make_shared<spdlog::logger>(FINNHUB_CLIENT_LOGGER_NAME, sink_std_cout);
	auto utility_logger = std::make_shared<spdlog::logger>(UTILITY_LOGGER_NAME, sink_std_cout);

	standard_logger->set_level(spdlog::level::trace);
	finnhub_client_logger->set_level(spdlog::level::debug);
	utility_logger->set_level(spdlog::level::info);

	spdlog::register_logger(standard_logger);
	spdlog::register_logger(finnhub_client_logger);
	spdlog::register_logger(utility_logger);
}


