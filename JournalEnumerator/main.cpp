// JournalEnumerator.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "core_api.hpp"

#include <memory>
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fmt/format.h>

int main()
{
	size_t total_file_count = 0;
	size_t volume_file_count = 0;
	std::vector<std::shared_ptr<zmystudio::core::file_record>> records;

	auto start_time = std::chrono::system_clock::now();

	for (auto& volume : zmystudio::core::get_logical_volumes())
	{
		std::cout << fmt::format("================ 分区 {} =====================", volume) << std::endl;
		volume_file_count = 0;

		auto volume_records = zmystudio::core::get_usn_journal_data(volume);

		std::for_each(std::begin(volume_records), std::end(volume_records), [&](const auto& item)
		{
			records.emplace_back(item);
			volume_file_count++;
		});

		total_file_count += volume_file_count;

		std::cout << fmt::format("文件数：{}", volume_file_count) << std::endl;
	}

	auto end_time = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.0;

	std::cout << "================ 结果 =====================" << std::endl;
	std::cout << fmt::format("总文件数：{}", total_file_count) << std::endl;
	std::cout << fmt::format("耗时：{} s", duration) << std::endl;

	system("pause");
}
