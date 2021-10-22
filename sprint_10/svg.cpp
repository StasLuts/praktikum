#include "svg.h"

namespace svg {

    using namespace std::literals;
    
    // ---------- OstreamColor ------------------

    void OstreamColor::operator()(std::monostate) const
    {
        out << NoneColor;
    }

    void OstreamColor::operator()(std::string color) const
    {
        out << color;
    }

    void OstreamColor::operator()(Rgb rgb) const
    {
        out << "rgb("sv
            << static_cast<int>(rgb.red) << ","sv
            << static_cast<int>(rgb.green) << ","sv
            << static_cast<int>(rgb.blue) << ")"sv;
    }

    void OstreamColor::operator()(Rgba rgba) const
    {
        out << "rgba("sv
            << static_cast<int>(rgba.red) << ","sv
            << static_cast<int>(rgba.green) << ","sv
            << static_cast<int>(rgba.blue) << ","
            << rgba.opacity << ")"sv;
    }

    // ---------- StrokeLineCap ------------------

    std::ostream& operator<<(std::ostream& os, const StrokeLineCap& line_cap)
    {
        switch (line_cap)
        {
        case StrokeLineCap::BUTT:
            os << "butt"sv;
            break;
        case StrokeLineCap::ROUND:
            os << "round"sv;
            break;
        case StrokeLineCap::SQUARE:
            os << "square"sv;
            break;
        }
        return os;
    }

    // ---------- StrokeLineJoin ------------------

    std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& line_join)
    {
        switch (line_join)
        {
        case StrokeLineJoin::ARCS:
            os << "arcs"sv;
            break;
        case StrokeLineJoin::BEVEL:
            os << "bevel"sv;
            break;
        case StrokeLineJoin::MITER:
            os << "miter"sv;
            break;
        case StrokeLineJoin::MITER_CLIP:
            os << "miter-clip"sv;
            break;
        case StrokeLineJoin::ROUND:
            os << "round"sv;
            break;
        }
        return os;
    }

    // ---------- Render ------------------

    void Object::Render(const RenderContext& context) const
    {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)
    {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)
    {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point)
    {
        points_.emplace_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (auto it = points_.begin(); it != points_.end(); ++it) {
            out << it->x << ","sv << it->y;
            if (it + 1 != points_.end()) {
                out << " "sv;
            }
        }
        out << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
        for (const auto& obj : objects_) {
            obj->Render(RenderContext(out, 2, 2));
        }
        out << "</svg>"sv;
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<text"sv;
        out << " x=\""sv << pos_.x
            << "\" y=\""sv << pos_.y << "\" "sv
            << "dx=\""sv << offset_.x
            << "\" dy=\""sv << offset_.y << "\" "sv
            << "font-size=\""sv << size_ << "\""sv;
        if (font_family_) {
            out << " font-family=\""sv << *font_family_ << "\""sv;
        }
        if (font_weight_) {
            out << " font-weight=\""sv << *font_weight_ << "\""sv;
        }
        out << ">"sv;
        ScreenString(out);
        out << "</text>"sv;
    }

    TypChar Text::GetTypeChar(char ch) const
    {
        switch (ch)
        {
        case '\"':

            return TypChar::QUOT;

        case '\'':

            return TypChar::APOS;

        case '<':

            return TypChar::LT;

        case '>':

            return TypChar::GT;

        case '&':

            return TypChar::AMP;

        default:

            return TypChar::OTHER;
        }
    }

    void Text::ScreenString(std::ostream& out) const
    {
        for (char ch : data_)
        {
            auto s = GetTypeChar(ch);
            switch (s)
            {
            case svg::TypChar::QUOT:

                out << "&quot;"sv;
                break;

            case svg::TypChar::APOS:

                out << "&apos;"sv;
                break;

            case svg::TypChar::LT:

                out << "&lt;"sv;
                break;

            case svg::TypChar::GT:

                out << "&gt;"sv;
                break;

            case svg::TypChar::AMP:

                out << "&amp;"sv;
                break;

            case svg::TypChar::OTHER:

                out << ch;
                break;
            }
        }
    }
    
}  // namespace svg