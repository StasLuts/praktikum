#include "request_handler.h"

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */

namespace request_handler
{
	RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer)
		: db_(db), renderer_(renderer) {}

	
} // namespace request_handler