#include "RedisPool.h"


#define THROW_REDIS_CONTEXT(message) throw RedisException(std::string(__FUNCTION__)+ \
									"|"+ redisContext_->errstr + "|" + std::string(message))
#define THROW_REDIS_REPLY(message) throw RedisException(std::string(__FUNCTION__)+ \
									"|"+ redisContext_->errstr + \
									"|" + reply->str + "|" + std::string(message))


RedisConnection::RedisConnection(RedisPool* redisPool)
	: redisContext_(NULL),
	  lastActiveTime_(time(NULL)),
	  redisPool_(redisPool)
{

}
RedisConnection::~RedisConnection()
{
	if (redisContext_) 
	{
		redisFree(redisContext_);
		redisContext_ = NULL;
	}
}

int RedisConnection::connect()
{
	struct timeval timeout = {0, 1000000}; // 1s
	redisContext_ = redisConnectWithTimeout(redisPool_->getServerIP(), redisPool_->getServerPort(), timeout);
	if (!redisContext_ || redisContext_->err) 
	{
		if (redisContext_) 
		{
			redisFree(redisContext_);
			redisContext_ = NULL;
		} 
		THROW_REDIS_CONTEXT("connect failed!");
	}

	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "SELECT %d", redisPool_->getDBNo()));
    if (!checkReply(reply)) 
    {
		if (reply)
			freeReplyObject(reply);
        THROW_REDIS_REPLY();
    }
	
	return 0;
}

bool RedisConnection::ping() const 
{
	redisReply *reply = static_cast<redisReply*>(redisCommand(redisContext_,"PING"));
	if (reply == NULL)
		return false;
	freeReplyObject(reply);
	
	return true;
}

bool RedisConnection::checkReply(const redisReply* reply)
{
    if(reply == NULL) 
        return false;

    switch(reply->type)
    {
    case REDIS_REPLY_STRING:
            return true;
    case REDIS_REPLY_ARRAY:
            return (strcasecmp(reply->str, "OK") == 0) ? true : false;
    case REDIS_REPLY_INTEGER:
            return true;
    case REDIS_REPLY_NIL:
            return false;
    case REDIS_REPLY_STATUS:
            return (strcasecmp(reply->str, "OK") == 0) ? true : false;
    case REDIS_REPLY_ERROR:
            return false;
    default:
            return false;
    }

    return false;
}

bool RedisConnection::set(std::string key, std::string &value)
{
    redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "SET %s %s", key.c_str(), value.c_str()));
    if (!checkReply(reply)) 
    {
		if (reply)
			freeReplyObject(reply);
        THROW_REDIS_REPLY();
    }

	freeReplyObject(reply);
    return true;
}

std::string RedisConnection::get(std::string key)
{
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "GET %s", key.c_str()));
    if (!checkReply(reply)) 
    {
		if (reply)
			freeReplyObject(reply);
        THROW_REDIS_REPLY();
    }

	std::string result;
	if (reply->type == REDIS_REPLY_STRING) 
		result.append(reply->str, reply->len);
	
	freeReplyObject(reply);
	return result;
}


RedisPool::RedisPool(const std::string ip, 
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
	  mutex_(),
	  notEmpty_(mutex_),
	  connections_()
{
	
}
RedisPool::~RedisPool()
{
	MutexLockGuard lock(mutex_);
	
	for (std::list<RedisConnection*>::iterator it = connections_.begin(); 
			it != connections_.end(); it++) 
	{
		delete *it;
	}

	connections_.clear();
	minConn_= 0;
}

int RedisPool::init()
{
	for (int i = 0; i < minConn_; i++) 
	{
		RedisConnection* conn = new RedisConnection(this);
		try
		{
			conn->connect(); 
		}
		catch(RedisException& ex)
		{
			if (conn) delete conn;
		}
		
		if (conn != NULL)
			connections_.push_back(conn);
	}

	return 0;
}

RedisConnection* RedisPool::getConnection()
{
	MutexLockGuard lock(mutex_);

	while (connections_.empty()) 
	{
		if (minConn_ >= maxConn_) 
		{
			notEmpty_.wait();
		} 
		else 
		{
			RedisConnection* conn = new RedisConnection(this);
			try
			{
				conn->connect();
			}
			catch(RedisException& ex)
			{
				if (conn) delete conn;
				throw;
			}
			connections_.push_back(conn);
			minConn_++;
		}
	}

	RedisConnection* pConn = connections_.front();
	connections_.pop_front();

	return pConn;
}

void RedisPool::freeConnection(RedisConnection* pCacheConn)
{
	MutexLockGuard lock(mutex_);

	std::list<RedisConnection*>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++) 
	{
		if (*it == pCacheConn) 
			break;
	}

	if (it == connections_.end()) 
	{
		connections_.push_back(pCacheConn);
	}

	notEmpty_.notify();
}


