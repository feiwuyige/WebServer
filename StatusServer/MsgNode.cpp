#include "MsgNode.h"

MsgNode::MsgNode(short max_len) : _total_len(max_len), _cur_len(0)
{
	//此处的 () 是用来进行值初始化
	_data = new char[_total_len + 1]();
	_data[_total_len] = '\0';
}

MsgNode::~MsgNode()
{
	std::cout << "destruct MsgNode " << std::endl;
	delete[] _data;
}

void MsgNode::Clear()
{
	std::memset(_data, 0, _total_len);
	_cur_len = 0;
}

RecvNode::RecvNode(short max_len, short msg_id) : MsgNode(max_len), _msg_id (msg_id)
{
}

SendNode::SendNode(const char* msg, short max_len, short msg_id) : 
	MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id)
{
	//先发送 id，转化为网络字节序
	short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
	memcpy(_data, &msg_id_host, HEAD_ID_LEN);
	short max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
	memcpy(_data + HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);
	memcpy(_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
}
