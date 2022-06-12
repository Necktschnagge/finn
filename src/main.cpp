#include "logger.h"

#include <cpr/cpr.h>

#include <iostream>

int main()
{
	init_logger();
	cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      cpr::Authentication{"user", "pass"},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    std::cout << r.status_code << '\n'                // 200
    << r.header["content-type"] << '\n'       // application/json; charset=utf-8
    << r.text;                         // JSON text string
    return 0;
}

