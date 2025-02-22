#include "ChatGrpcClient.h"
#include "RedisMgr.h"
#include "ConfigMgr.h"
#include "UserMgr.h"
#include "CSession.h"
#include "MysqlMgr.h"

ChatConPool::ChatConPool(size_t poolSize, std::string host, std::string port) :
	_b_stop(false), _pool_size(poolSize), _host(host), _port(port)
{
	for (size_t i = 0;i < poolSize;++i) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());         //����һ��ͨ��
		_connections.push(ChatService::NewStub(channel)); //�����channel��װ��ʹ����ͨ��
	}
}

ChatConPool::~ChatConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Close();
	while (!(_connections.empty())) {
		_connections.pop();
	}
}

std::unique_ptr<ChatService::Stub> ChatConPool::getConnection()
{
	//�������ٽ���Դ������
	std::unique_lock<std::mutex> lock(_mutex);
	//���ж�ν�ʣ���Ϊfalse, �ͷ���������˯�ߣ�������ʱ�ж�ν��Ϊ true �����ִ��
	_cond.wait(lock, [this]() {
		if (_b_stop) {
			return true;
		}
		return !_connections.empty();
		});
	if (_b_stop) return nullptr;
	auto context = std::move(_connections.front()); //unique_ptr��ֹ����
	_connections.pop();
	return context;
}

void ChatConPool::returnConnection(std::unique_ptr<ChatService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) return;
	_connections.push(std::move(context));
	_cond.notify_one();
}

void ChatConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

ChatGrpcClient::~ChatGrpcClient()
{
}

AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req)
{
	//todo
	return AddFriendRsp();
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req)
{
	//todo
	return AuthFriendRsp();
}

bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	//todo
	return false;
}

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, 
	const Json::Value& rtvalue)
{
	//todo
	return TextChatMsgRsp();
}

ChatGrpcClient::ChatGrpcClient()
{
	auto& cfg = ConfigMgr::Inst();
	auto server_list = cfg["PeerServer"]["Servers"];

	std::vector<std::string> words;
	std::stringstream ss(server_list);
	std::string word;

	while (std::getline(ss, word, ',')) {
		words.push_back(word);
	}
	for (auto& word : words) {
		if (cfg[word]["Name"].empty()) {
			continue;
		}
		_pools[cfg[word]["Name"]] = std::make_unique<ChatConPool>(5, cfg[word]["Host"], cfg[word]["Port"]);
	}
}
