#include "transport_router.h"

namespace transport_router
{
	const transport_router::RoureInfo& TransportRouter::FindRoute(const std::string_view from, const std::string_view to) const
	{
		domain::StopPtr from = trans_cat_.FindStop(from);
		domain::StopPtr to = trans_cat_.FindStop(to);
	}
} // namespace transport_router