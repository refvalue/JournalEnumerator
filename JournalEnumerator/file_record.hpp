#pragma once

#include "file_id.hpp"

namespace zmystudio::core
{
	struct file_record
	{
		file_id id;
		file_id parent_id;
		std::string file_name;
	};
}
