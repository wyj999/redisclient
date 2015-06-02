#include "RedisClient.h"


RedisClient::RedisClient(const std::string ip, 
					uint16_t port, 
					int minConn,
					int maxConn, 
					int dbNo,
					const std::string nameArg)
	: hostip_(ip),
	  hostport_(port),
	  minConn_(minConn),
	  maxConn_(maxConn),
      dbNo_(dbNo),
	  name_(nameArg),
	  redisPool_(new RedisPool(ip, port, minConn, maxConn, dbNo,  nameArg)) 
{
	redisPool_->init();
}


RedisClient::~RedisClient() 
{
	if (redisPool_)
	{
		delete redisPool_;
		redisPool_ = NULL;
	}
}

void RedisClient::set(std::string key, std::string value)
{
    RedisConnection* conn = redisPool_->getConnection();

	conn->set(key, value);

    redisPool_->freeConnection(conn);
}

std::string RedisClient::get(std::string key)
{
    RedisConnection* conn = redisPool_->getConnection();

	std::string value = conn->get(key);

	redisPool_->freeConnection(conn);
	
	return value;
}


