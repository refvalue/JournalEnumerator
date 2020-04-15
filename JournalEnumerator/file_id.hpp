#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <compare>

namespace zmystudio::core
{
	struct file_id
	{
		static constexpr size_t id_size = 16;

		std::array<std::byte, id_size> id;

		file_id(uint8_t(&data)[id_size]);
		file_id(std::byte(&data)[id_size]);
		auto operator<=>(const file_id& right) const = default;
	};
}
