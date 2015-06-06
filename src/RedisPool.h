#ifndef REDIS_POOL_H
#define REDIS_POOL_H

#include "hiredis.h"
#include "RedisSynch.h"
#include "RedisException.h"
#include <list>
#include <new>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <stdint.h>
#include <string.h>
#include <thread>
#include <functional>
#include <chrono>



class RedisPool;

class RedisConnection
{
public:
	RedisConnection(RedisPool* redisPool);
	~RedisConnection();

	int connect();
	bool checkReply(const redisReply *reply);
	
	bool ping();
	bool exists(std::string key);
    bool set(std::string key, std::string &value);
    std::string get(std::string key);
	int hset(std::string key, std::string field, std::string value);
	std::string hget(std::string key, std::string field);
private:
	
	redisContext* 	redisContext_;
	uint64_t lastActiveTime_;
    std::string errstr_;
	RedisPool* redisPool_;
};

class RedisPool
{
public:
	RedisPool(const std::string ip, 
					uint16_t port, 
					int minConn,
	  				int maxConn,
					int dbNo,
					const std::string nameArg);
	~RedisPool();

	int init();
	void serverCron();

	int getDBNo() { return dbNo_; }

	const char* getServerIP() { return hostip_.c_str(); }
	int getServerPort() { return hostport_; }

    RedisConnection* getConnection();
    void freeConnection(RedisConnection*);
private:
	const std::string hostip_;
	uint16_t hostport_;
	int minConn_;
	int maxConn_;
	int dbNo_;
	const std::string name_;
	mutable MutexLock mutex_;  
	Condition notEmpty_;
	std::list<RedisConnection*>	connections_;
	std::thread* cronThread;
	bool quit_;
};
#endif // REDIS_POOL_H
