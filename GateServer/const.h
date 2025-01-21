#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include <functional>
#include "Singleton.h"
#include <map>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//∂®“Â¥ÌŒÛ
enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,
};