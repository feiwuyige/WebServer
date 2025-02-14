#include "AsioIOServicePool.h"
#include <iostream>

AsioIOServicePool::~AsioIOServicePool()
{
	Stop();
	std::cout << "AsioIOServicePool destruct" << std::endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	auto& service = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop()
{
	//仅仅执行work.reset并不能让iocontext从run的状态退出
	//当 iocontext 已经绑定了读或写的监听事件以后，需要手动 stop 该服务
	for (auto& work : _works) {
		//停止服务
		work->get_io_context().stop();
		work.reset();
	}
	//work退出以后，iocontext.run发现没有读写事件就会退出执行
	for (auto& t : _threads) {
		t.join();
	}
}

AsioIOServicePool::AsioIOServicePool(std::size_t size) : _ioServices(size),
	_works(size), _nextIOService(0)
{
	for (std::size_t i = 0;i < size;++i) {
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));
	}
	//遍历多个 ioservice，创建多个线程，每个线程内部启动ioservice
	for (std::size_t i = 0;i < _ioServices.size(); ++i) {
		_threads.emplace_back([this, i]() {
			_ioServices[i].run();
		});
	}
}
