#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libmemcached/memcached.h>

memcached_st * init_env(unsigned char *ip, unsigned short port)
{
	//表示和memcached通讯的句柄
	memcached_st *memc;
	
	//表示调用API返回的类型
	memcached_return rc;
	
	//memcached client 需要链接的server句柄
	memcached_server_st *server = NULL;
	

	//创建一个memcached句柄
	memc = memcached_create(NULL);
	
	//创建一个server
	server = memcached_server_list_append(server, ip, port, &rc);
	//将创建好的server列表 添加到memcached句柄中
	rc=memcached_server_push(memc,server);
	
	if (MEMCACHED_SUCCESS != rc)
	{
		printf("memcached_server_push failed! rc: %d\n", rc);
	} 
	
	//server就不需要了
	memcached_server_list_free(server);
	
	return memc;
}

int insert(memcached_st* memc, const char* key, const char* value, time_t expiration)
{
	if (NULL == key || NULL == value) 
	{
		return -1;
	} 
	
	uint32_t flags = 0;
	memcached_return rc;
	
	//expiration代表缓存时间，以秒为单位
	//最后一个参数已经废弃
	rc = memcached_set(memc, key, strlen(key),value,
		strlen(value), expiration, flags);
	
	// insert ok
	if (MEMCACHED_SUCCESS == rc) 
	{
		return 1;
	} 
	else 
	{
		return 0;
	}
}
	
char * get(memcached_st* memc, const char* key)
{
	if (NULL == key) 
	{
		return "";
	} 

	uint32_t flags = 0;
	memcached_return rc;
	size_t value_length;
	//key的值， value无需释放
	char* value = memcached_get(memc, key, strlen(key),
		&value_length, &flags, &rc);
		
	// get ok
	if(rc == MEMCACHED_SUCCESS) 
	{
		return value;
	}
	
	return "";
}

int main()
{
	//表示和memcached通讯的句柄
	memcached_st *memc;
	memc = init_env("127.0.0.1", 11211);
	
	//设置key-value
	insert(memc, "mike", "jiang", 0);
	
	//获取key所对应的value
	const char* value = get(memc, "mike");
	printf("value = %s\n", value);
	
	
	memcached_free(memc);
		
		
	return 0;
}