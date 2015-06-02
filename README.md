# RedisClient
  A redis client library for C++ with connection pool
## Build
```bash
  yum install libhiredis-dev
  git clone https://github.com/SourceInsight/RedisClient
  cd RedisClient
  ./build.sh 
```
## Usage
```bash
  // ip, port, min conneciton, max connection, db No, pool name
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
```
## Todo
  lots of work to do ....
