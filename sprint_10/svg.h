#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <deque>
#include <optional>
#include <variant>

namespace svg
{
    // ---------- Rgb ------------------
    
    struct Rgb
    {
        Rgb() = default;

        Rgb(uint8_t r, uint8_t g, uint8_t b)
            : red(r), green(g), blue(b) {}

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    // ---------- Rgba ------------------

    struct Rgba
    {
        Rgba() = default;

        Rgba(uint8_t r, uint8_t g, uint8_t b, double a)
            : red(r), green(g), blue(b), opacity(a) {}

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0;
    };

    // ---------- OstreamColor ------------------

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

    inline const std::string NoneColor{ "none" };
    
    struct OstreamColor
    {
        void operator()(std::monostate) const;
        void operator()(std::string color) const;
        void operator()(Rgb rgb) const;
        void operator()(Rgba rgba) const;

        std::ostream& out;
    };

    // ---------- StrokeLineCap ------------------

    enum class StrokeLineCap
    {
        BUTT,
        ROUND,
        SQUARE,
    };

    std::ostream& operator<<(std::ostream& os, const StrokeLineCap& line_cap);

    // ---------- StrokeLineJoin ------------------

    enum class StrokeLineJoin
    {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& line_join);

    // ---------- PathProps ------------------

    template <typename Owner>
    class PathProps
    {

    public:

        Owner& SetFillColor(Color color)
        {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color)
        {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(const double width)
        {
            width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap)
        {
            line_cap_ = std::move(line_cap);
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join)
        {
            line_join_ = std::move(line_join);
            return AsOwner();
        }

    protected:

        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_)
            {
                out << " fill=\""sv;
                std::visit(OstreamColor{ out }, *fill_color_);
                out << "\""sv;
            }
            if (stroke_color_)
            {
                out << " stroke=\""sv;
                std::visit(OstreamColor{ out }, *stroke_color_);
                out << "\""sv;
            }
            if (width_)
            {
                out << " stroke-width=\""sv
                    << *width_ << "\""sv;
            }
            if (line_cap_)
            {
                out << " stroke-linecap=\""sv
                    << *line_cap_ << "\""sv;
            }
            if (line_join_)
            {
                out << " stroke-linejoin=\""sv
                    << *line_join_ << "\""sv;
            }
        }

    private:

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;

        Owner& AsOwner()
        {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }
    };

    // ---------- Point ------------------

    struct Point
    {
        Point() = default;

        Point(double x, double y)
            : x(x), y(y) {}

        double x = 0;
        double y = 0;
    };

    // ---------- RenderContext ------------------

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext
    {
        RenderContext(std::ostream& out)
            : out(out) {}

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out), indent_step(indent_step), indent(indent) {}

        RenderContext Indented() const
        {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const
        {
            for (int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    // ---------- Object ------------------

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    // ---------- Circle ------------------

    class Circle final : public Object, public PathProps<Circle>
    {
    public:

        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:

        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    // ---------- Polyline ------------------

    class Polyline final : public Object, public PathProps <Polyline>
    {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:

        void RenderObject(const RenderContext& context) const override;

        std::deque<Point> points_;
    };

    // ---------- Text ------------------

    enum class TypChar
    {
        QUOT,
        APOS,
        LT,
        GT,
        AMP,
        OTHER
    };

    class Text final : public Object, public PathProps<Text>
    {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

    private:

        void RenderObject(const RenderContext& context) const override;

        TypChar GetTypeChar(char ch) const;

        void ScreenString(std::ostream& out) const;

        Point pos_;
        Point offset_;
        uint32_t size_ = 1;
        std::optional<std::string> font_family_;
        std::optional<std::string> font_weight_;
        std::string data_;
    };

    // ---------- ObjectContainer ------------------

    class ObjectContainer
    {
    public:

        template<typename T>
        void Add(T obj)
        {
            objects_.emplace_back(std::make_shared<T>(std::move(obj)));
        }

        virtual void AddPtr(std::shared_ptr<Object>&& obj) = 0;

    protected:

        virtual ~ObjectContainer() = default;

        std::deque<std::shared_ptr<Object>> objects_;
    };

    // ---------- Document ------------------

    class Document final : public ObjectContainer {
    public:

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::shared_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;
    };

    // ---------- Drawable ------------------

    class Drawable
    {
    public:

        virtual void Draw(ObjectContainer& container) const = 0;

        virtual ~Drawable() = default;
    };

}  // namespace svg