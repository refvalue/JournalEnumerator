#include "core_api.hpp"
#include "aggregate_wrapper.hpp"

#include <memory>

#include <Windows.h>
#include <winioctl.h>
#include <fmt/format.h>

namespace zmystudio::core
{
	namespace
	{
		thread_local std::byte read_buffer[4096];

		constexpr int code_page_gbk = 936;
		constexpr std::string_view delimiter{ "\\\0", 2 };
	}

	std::string utf16_to_gbk(const std::wstring_view& str)
	{
		std::string result;
		int size = WideCharToMultiByte(code_page_gbk, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);

		result.resize(size);
		WideCharToMultiByte(code_page_gbk, 0, str.data(), static_cast<int>(str.size()), result.data(), static_cast<int>(result.size()), nullptr, nullptr);

		return result;
	}

	std::experimental::generator<std::string> get_logical_volumes()
	{
		uint32_t size = GetLogicalDriveStringsA(0, nullptr);

		if (size == 0)
		{
			co_return;
		}

		std::string volumes;

		volumes.resize(size);

		if (GetLogicalDriveStringsA(size, volumes.data()) == 0)
		{
			co_return;
		}

		size_t start_pos = 0;
		size_t end_pos = std::string::npos;

		while ((end_pos = volumes.find_first_of(delimiter, start_pos)) != std::string::npos)
		{
			auto volume = volumes.substr(start_pos, end_pos - start_pos);

			if (GetDriveTypeA(volume.c_str()) == DRIVE_FIXED)
			{
				co_yield fmt::format(R"(\\.\{})", volume);
			}

			start_pos = end_pos + delimiter.size();
		}
	}

	std::experimental::generator<std::shared_ptr<file_record>> get_usn_journal_data(const std::string& volume)
	{
		HANDLE volume_handle = CreateFileA(volume.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);

		if (volume_handle == INVALID_HANDLE_VALUE)
		{
			co_return;
		}

		std::shared_ptr<void> volume_handle_scope{ volume_handle, &CloseHandle };

		DWORD bytes_read;
		USN_JOURNAL_DATA journal_data;

		if (!DeviceIoControl(volume_handle, FSCTL_QUERY_USN_JOURNAL, nullptr, 0, &journal_data, sizeof(journal_data), &bytes_read, nullptr))
		{
			co_return;
		}

		DWORD bytes_remaining;
		PUSN_RECORD_V3 record;
		MFT_ENUM_DATA enum_data = { 0, 0, journal_data.NextUsn, 3, 3 };

		for (;;)
		{
			memset(read_buffer, 0, sizeof(read_buffer));

			if (!DeviceIoControl(volume_handle, FSCTL_ENUM_USN_DATA, &enum_data, sizeof(enum_data), read_buffer, sizeof(read_buffer), &bytes_read, nullptr))
			{
				break;
			}

			bytes_remaining = bytes_read - sizeof(USN);
			record = reinterpret_cast<PUSN_RECORD_V3>(read_buffer + sizeof(USN));

			while (bytes_remaining > 0)
			{
				co_yield std::make_shared<aggregate_wrapper<file_record>>(record->FileReferenceNumber.Identifier, record->ParentFileReferenceNumber.Identifier, utf16_to_gbk(record->FileName));

				bytes_remaining -= record->RecordLength;
				record = reinterpret_cast<PUSN_RECORD_V3>(reinterpret_cast<std::byte*>(record) + record->RecordLength);
			}

			enum_data.StartFileReferenceNumber = *reinterpret_cast<USN*>(read_buffer);
		}
	}
}
