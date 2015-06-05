#ifndef REDIS_CLIENT_H
#define REDIS_CLIENT_H

#include "hiredis.h"
#include "RedisPool.h"

class RedisClient
{
public:
	RedisClient(const std::string ip, 
					uint16_t port, 
					int minConn = 1,
					int maxConn = 8, 
                    int dbNo = 0,
					const std::string nameArg = std::string());
	~RedisClient();

	int init();

	// Connection
	// Key	
	// String
    void set(std::string key, std::string value);
	std::string get(std::string key);
	// Hash
	int hset(std::string key, std::string field, std::string value);
	std::string hget(std::string key, std::string field);
	// List
	// Set
	

private:
	const std::string hostip_;
	uint16_t hostport_;
	int minConn_;
	int maxConn_;
	int dbNo_;
	const std::string name_;
	RedisPool* redisPool_;
};

//typedef std::shared_ptr<redis::RedisClient> RedisClientPtr;

#endif // REDIS_CLIENT_H
