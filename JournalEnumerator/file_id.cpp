#include "file_id.hpp"

namespace zmystudio::core
{
	file_id::file_id(uint8_t(&data)[id_size]) : file_id{ reinterpret_cast<std::byte(&)[id_size]>(data) }
	{
	}

	file_id::file_id(std::byte(&data)[id_size]) : id{}
	{
		for (size_t i = 0; i < id_size; i++)
		{
			id[i] = data[i];
		}
	}
}
