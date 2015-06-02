#ifndef REDIS_POOL_H
#define REDIS_POOL_H

#include "hiredis.h"
#include "RedisSynch.h"
#include "RedisException.h"
#include <list>
#include <new>
#include <stdint.h>
#include <string>
#include <stdint.h>
#include <string.h>


using std::string;

class RedisPool;

class RedisConnection
{
public:
	RedisConnection(RedisPool* redisPool);
	~RedisConnection();

    int connect();
	bool ping() const;
	bool checkReply(const redisReply *reply);
	std::string getErrText() {return errstr_;}
	bool setErrText(const char *info);

    bool set(std::string key, std::string &value);
    std::string get(std::string key);
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
};
#endif // REDIS_POOL_H
