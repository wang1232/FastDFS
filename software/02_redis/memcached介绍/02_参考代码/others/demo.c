#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libmemcached/memcached.h>
#include <pthread.h>

void *do_search(void *arg)
{
	char *key = "key2";
	uint32_t flags = 0;
	size_t value_length = 0;
	memcached_return rc;
	memcached_st *memc = (memcached_st *)arg;
	
	sleep(2);
	while (1) 
	{
		//循环取key2的值， value无需释放
		char *value = memcached_get(memc, key, strlen(key),\
			&value_length, &flags, &rc);
		if (rc == MEMCACHED_SUCCESS) 
		{
			printf("key:%s\tvalue:%s\tvalue_lenth:%ld\n", \
				key, value, value_length); 
		}
		else
		{
			printf("unserach or error\n");
		}
	
		printf("search rc: %s\n", memcached_strerror(memc, rc));
		sleep(5);
	}
} 

int main(int argc, char *argv[])
{
	//表示和memcached通讯的句柄
	memcached_st *memc;
	//表示调用API返回的类型
	memcached_return rc;
	
	//memcached client 需要链接的server句柄
	memcached_server_st *servers;
	
	//创建一个memcached句柄
	memc = memcached_create(NULL);

	//由于是分布式的，要创建多个server
	//创建一个server1
	servers = memcached_server_list_append(NULL, (char *)"127.0.0.1", 11222, &rc);
	
	//创建一个server2
	servers = memcached_server_list_append(servers, (char *)"127.0.0.1", 11322, &rc);
	
	//将创建好的server列表 添加到memcached句柄中
	rc = memcached_server_push(memc, servers);
	
	//server就不需要了
	memcached_server_free(servers);
	
	// 分布 一致性，如果只有一台服务器，下面不用设置
	//保证2个server的数据时相互备份的，一致
	//MEMCACHED_BEHAVIOR_DISTRIBUTION：代表分布式
	//MEMCACHED_DISTRIBUTION_CONSISTENT：代表相互协调，一致，纵向拓展容
	memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_DISTRIBUTION,MEMCACHED_DISTRIBUTION_CONSISTENT);
	
	//设置一个服务器，超时时间为20秒，最长等待20秒
	memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 20);
	//设置如果一个server出现问题，就将此server从memc句柄中拆除
	memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, 1);
	//允许服务器出错最大个数
	memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 5);
	//允许自动连接主机
	memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, true);
	
	pthread_t tid;
	pthread_create(&tid, NULL, do_search, (void *)memc); //创建线程
	
	int time_s1 = 0;
	int times = 0;
	const char *keys[] = {"key1", "key2", "key3", "key4"}; //key值
	const size_t key_length[] = {4, 4, 4, 4};//每个key值长度
	char *values[] = {"first", "second", "third", "forth"}; //value值
	size_t val_length[] = {5, 6, 5, 5};//每个value的长度
	
	int i;
	//存放4个key, 存放1万次
	while (times++ < 100000) 
	{
		for (i = 0; i < 4; i++) 
		{
			//180代表缓存时间，180秒，3分钟
			//最后一个参数已经废弃
			rc = memcached_set(memc, keys[i], key_length[i],
				values[i], val_length[i],(time_t)180, (uint32_t)0);
			
			printf("key: %s rc: %s\n", keys[i], memcached_strerror(memc, rc));
		} 
		
		printf("time : %d\n", time_s1++);
		sleep(1);
	} 

	memcached_free(memc);
	
	return 0;
}