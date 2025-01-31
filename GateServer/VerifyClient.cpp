#include "VerifyClient.h"
#include "ConfigMgr.h"
RPConPool::RPConPool(std::size_t poolsize, std::string host, std::string port) :
	pool_size_(poolsize), host_(host), port_(port), b_stop_(false){
	for (std::size_t i = 0; i < pool_size_;++i) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());
		connections_.push(VerifyService::NewStub(channel));
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(mutex_);
	Close();
	while (!connections_.empty()) {
		connections_.pop();
	}
}

void RPConPool::Close()
{
	b_stop_ = true;
	cond_.notify_all();
}

std::unique_ptr<VerifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this]() {
		if (b_stop_) {
			return true;
		}
		return !connections_.empty();
		});
	if (b_stop_) {
		return nullptr;
	}
	auto context = std::move(connections_.front());
	connections_.pop();
	return context;
}

void RPConPool::returnConnection(std::unique_ptr<VerifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	connections_.push(std::move(context));
	cond_.notify_one();
}

VerifyClient::VerifyClient()
{
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VerifyServer"]["Host"];
	std::string port = gCfgMgr["VerifyServer"]["Port"];
	pool_.reset(new RPConPool(5, host, port));
}
