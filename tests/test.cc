#include <iostream>
#include "RedisClient.h"

int main()
{
	RedisClient client("127.0.0.1", 6379, 2, 10, 5, "Test");
	try
	{
		client.set("name","Michael");
		std::cout << client.get("name") << std::endl;
	}
	catch (const RedisException& ex)
	{
		std::cout << "Reason: " << ex.what() << std::endl;
	}
	
}
