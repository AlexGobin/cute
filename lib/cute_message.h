#ifndef CUTE_MESSAGE_H__
#define CUTE_MESSAGE_H__

#include "config.h"

class cute_mem_pool;
class cute_data_block;

class cute_message
{
public:
	cute_message(cute_mem_pool* pool, i32 len);
	~cute_message();

	cute_message(cute_message&& src);
        cute_message&& operator = (cute_message&& src);

public:
	cute_message(const cute_message&) = delete;
        cute_message& operator = (const cute_message&) = delete;


public:
	template<typename... T>
	i32 read(T&&... data)
	{
		return this->read_i(std::forward<T>(data)...);
	}

	template<typename... T>
	i32 write(T&&... data)
	{
		return this->write_i(std::forward<T>(data)...);
	}
	
private:
	i32 read_i(u8& data);
	i32 read_i(u16& data);
	i32 read_i(u32& data);
	i32 read_i(u64& data);

	i32 read_i(i8& data);
	i32 read_i(i16& data);
	i32 read_i(i32& data);
	i32 read_i(i64& data);

	i32 write_i(u8 data);
	i32 write_i(u16 data);
	i32 write_i(u32 data);
	i32 write_i(u64 data);

	i32 write_i(i8 data);
	i32 write_i(i16 data);
	i32 write_i(i32 data);
	i32 write_i(i64 data);

private:
	i32 write_bytes_i(u8 * data, u32 len);
	i32 next_write_block();

	i32 read_bytes_i(u8* data, u32 len);
	i32 next_read_block();


private:
	cute_mem_pool*			pool_;
	std::vector<cute_data_block>	data_block_vec_;
	u32				write_vec_idx_;
	u32				read_vec_idx_;
};

#endif

