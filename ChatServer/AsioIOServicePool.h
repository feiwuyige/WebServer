#pragma once
#include <vector>
#include <boost/asio.hpp>
#include "Singleton.h"
class AsioIOServicePool : public Singleton<AsioIOServicePool>
{
	friend Singleton<AsioIOServicePool>;
public:
	using IOService = boost::asio::io_context;
	//�� work �� iocontext �����ʹ�� io_context ��û���¼���ʱ��Ҳ�����˳�
	using Work = boost::asio::io_context::work;
	using WorkPtr = std::unique_ptr<Work>;
	~AsioIOServicePool();
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	const AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
	boost::asio::io_context& GetIOService();
	void Stop();
private:
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());
	std::vector<IOService> _ioServices;
	std::vector<WorkPtr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextIOService;
};

