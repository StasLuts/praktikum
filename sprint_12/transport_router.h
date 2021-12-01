#pragma once

#include "transport_catalogue.h"
#include "domain.h"
#include "router.h"

namespace transport_router
{

	struct RoureInfo
	{
		double total_time = 0;
	};

	class TransportRouter
	{
	public:

		TransportRouter(const transport_catalogue::TransportCatalogue& trans_cat)
			: trans_cat_(trans_cat) {}

		const transport_router::RoureInfo& FindRoute(const std::string_view from, const std::string_view to) const;

	private:

		const transport_catalogue::TransportCatalogue& trans_cat_;
	};
} // namespace transport_router
