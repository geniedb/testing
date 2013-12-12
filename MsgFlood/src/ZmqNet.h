#ifndef ZMQ_NET_H_
#define ZMQ_NET_H_

#include <string>
#include "CF_Platform.h"
#include "zmq.hpp"
#include "Net.h"

namespace ZmqNet {
	
class ZmqSocket {
friend class ZmqNetImpl;
protected:
	zmq::socket_t* socket;
	ZmqSocket(zmq::socket_t* socket);
public:
	~ZmqSocket();
	bool connected();
	void close();
};

class ZmqPublishSocket : public Model::PublishSocket, public ZmqSocket {
friend class ZmqNetImpl;
private:
	ZmqPublishSocket(zmq::socket_t* socket) : ZmqSocket(socket) {}
public:
	virtual bool send(Model::Message* message);
	virtual bool connected();
	virtual void close();
};

class ZmqSubscribeSocket : public Model::SubscribeSocket, private ZmqSocket {
friend class ZmqNetImpl;
private:
	zmq::pollitem_t pollitem;
	ZmqSubscribeSocket(zmq::socket_t* socket);
public:
	virtual Model::Message* receive();
	virtual bool connected();
	virtual void close();
};

class ZmqNetImpl : public Model::Net {
private:
	zmq::context_t context;
public:
	ZmqNetImpl(): context(1) {};
	virtual Model::PublishSocket* connectPublish(std::string url, int port, Model::NetSettings& netSettings);
	virtual Model::SubscribeSocket* connectSubscribe(std::string subject, std::string url, int port, Model::NetSettings& netSettings);
	virtual void close();
	virtual Model::Message* createMessage(Model::MessageType messageType, char* data, size_t dataSize, uint64_t counter);
};

}

#endif /* ZMQ_NET_H_ */