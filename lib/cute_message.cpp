#include "cute_mem_pool.h"
#include "cute_data_block.h"
#include "cute_message.h"

cute_message::cute_message(cute_mem_pool* pool, i32 len)
	: pool_(pool)
	, write_vec_idx_(0)
	, read_vec_idx_(0)
{
	u32 node_payload_len = pool->node_payload_size();
	u32 node_cnt = len / node_payload_len;
	if (len % node_payload_len)
		++node_cnt;
	for (u32 k = 0; k < node_cnt; ++k)
	{
		auto block = cute_data_block(this->pool_);
		if (CUTE_SUCC == block.acquire())
			this->data_block_vec_.push_back(block);	
	}
}

cute_message::~cute_message()
{
	std::for_each(this->data_block_vec_.begin(), this->data_block_vec_.end(), [](cute_data_block& block)
	{
		block.release();
 	});
	this->data_block_vec_.clear();
}

cute_message::cute_message(cute_message&& src)
: pool_(src.pool_)
, data_block_vec_(std::move(src.data_block_vec_))
, write_vec_idx_(src.write_vec_idx_)
, read_vec_idx_(src.read_vec_idx_)
{
	src.pool_ = nullptr;
	src.write_vec_idx_ = 0;
	src.read_vec_idx_ = 0;
}

cute_message&& cute_message::operator = (cute_message&& src)
{
	this->pool_ = src.pool_;
	this->data_block_vec_ = std::move(src.data_block_vec_);
	this->write_vec_idx_ = src.write_vec_idx_;
	this->read_vec_idx_ = src.read_vec_idx_;

	src.pool_ = nullptr;
	src.write_vec_idx_ = 0;
	src.read_vec_idx_ = 0;
	return std::move(*this);	
}

i32 cute_message::read_i(u8& data)
{
	return this->read_bytes_i((u8*)&data, u8_size);
}

i32 cute_message::read_i(u16& data)
{
	return this->read_bytes_i((u8*)&data, u16_size);
}

i32 cute_message::read_i(u32& data)
{
	return this->read_bytes_i((u8*)&data, u32_size);
}

i32 cute_message::read_i(u64& data)
{
	return this->read_bytes_i((u8*)&data, u64_size);
}

i32 cute_message::read_i(i8& data)
{
	return this->read_bytes_i((u8*)&data, u8_size);
}

i32 cute_message::read_i(i16& data)
{
	return this->read_bytes_i((u8*)&data, i16_size);
}

i32 cute_message::read_i(i32& data)
{
	return this->read_bytes_i((u8*)&data, i32_size);
}

i32 cute_message::read_i(i64& data)
{
	return this->read_bytes_i((u8*)&data, i64_size);
}

i32 cute_message::next_read_block()
{
	++this->read_vec_idx_;
	return this->data_block_vec_.size() > this->read_vec_idx_ ? CUTE_SUCC : CUTE_ERR;
}

i32 cute_message::read_bytes_i(u8* data, u32 len)
{
	if (this->read_vec_idx_ >= this->data_block_vec_.size())
		return CUTE_ERR;
	auto ret = this->data_block_vec_[this->read_vec_idx_].read((u8*)data, len);
	if (cute_data_block::NO_DATA_TO_READ == ret)
		return CUTE_ERR;
	else if (CUTE_SUCC == ret)
		return CUTE_SUCC;
	else
	{
		this->next_read_block();
		if (cute_data_block::READ_TO_END == ret)
			return this->read_bytes_i(data, len);
		else
			return this->read_bytes_i(data + ret, len - ret);
	}
}

i32 cute_message::write_i(u8 data)
{
	return this->write_bytes_i((u8*)&data, u8_size);
}

i32 cute_message::write_i(u16 data)
{
	return this->write_bytes_i((u8*)&data, u16_size);
}

i32 cute_message::write_i(u32 data)
{
	return this->write_bytes_i((u8*)&data, u32_size);
}

i32 cute_message::write_i(u64 data)
{
	return this->write_bytes_i((u8*)&data, u64_size);
}

i32 cute_message::write_i(i8 data)
{
	return this->write_bytes_i((u8*)&data, i8_size);
}

i32 cute_message::write_i(i16 data)
{
	return this->write_bytes_i((u8*)&data, i16_size);
}

i32 cute_message::write_i(i32 data)
{
	return this->write_bytes_i((u8*)&data, i32_size);
}

i32 cute_message::write_i(i64 data)
{
	return this->write_bytes_i((u8*)&data, i64_size);
}


i32 cute_message::write_bytes_i(u8* data, u32 len)
{
	if (this->write_vec_idx_ >= this->data_block_vec_.size())
		return CUTE_ERR;
	
	auto ret = this->data_block_vec_[this->write_vec_idx_].write((u8*)data, len);
	if (CUTE_SUCC == ret)
		return ret;
	else
	{
		this->next_write_block();
		if (cute_data_block::WRITE_TO_END == ret)
			return this->write_bytes_i(data, len);
		else
			return this->write_bytes_i(data + ret, len - ret);
	}
}

i32 cute_message::next_write_block()
{
        ++this->write_vec_idx_;
        return this->data_block_vec_.size() > this->write_vec_idx_ ? CUTE_SUCC : CUTE_ERR;
}

