#include "map_renderer.h"

/*
 * � ���� ����� �� ������ ���������� ���, ���������� �� ������������ ����� ��������� � ������� SVG.
 * ������������ ���������� ��� ����������� �� ������ ����� ��������� �������.
 * ���� ������ �������� ���� ������.
 */

namespace map_renderer
{
	//----------------SphereProjector----------------------------

	bool IsZero(double value)
	{
		return std::abs(value) < EPSILON;
	}

	svg::Point SphereProjector::operator()(geo::Coordinates coords) const
	{
		return { (coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
	}

	//----------------SphereProjector----------------------------

	void MapRenderer::SetRenderSettings(const RenderSettings& render_settings)
	{
		render_settings_ = render_settings;
		/******/
	}
} // namespace map_renderer