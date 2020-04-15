#pragma once

#include <type_traits>

namespace zmystudio
{
	template<typename TBase>
	struct aggregate_wrapper : TBase
	{
		template<typename... TArgs>
		aggregate_wrapper(TArgs&&... args) : TBase{ std::forward<TArgs>(args)... }
		{
		}
	};
}
