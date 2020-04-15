#pragma once

#include "file_record.hpp"

#include <memory>
#include <string>
#include <experimental/generator>

namespace zmystudio::core
{
	std::string utf16_to_gbk(const std::wstring_view& str);
	std::experimental::generator<std::string> get_logical_volumes();
	std::experimental::generator<std::shared_ptr<file_record>> get_usn_journal_data(const std::string& volume);
}
