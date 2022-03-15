#include "runtime.h"

#include <cassert>
#include <optional>
#include <sstream>

using namespace std::literals;

namespace runtime
{
    //-------------------ObjectHolder-------------------------

    ObjectHolder ObjectHolder::Share(Object& object)
    {
        // Возвращаем невладеющий shared_ptr (его deleter ничего не делает)
        return ObjectHolder(std::shared_ptr<Object>(&object, [](auto* /*p*/) { /* do nothing */ }));
    }

    ObjectHolder ObjectHolder::None()
    {
        return ObjectHolder();
    }

    Object& ObjectHolder::operator*() const
    {
        AssertIsValid();
        return *Get();
    }

    Object* ObjectHolder::operator->() const
    {
        AssertIsValid();
        return Get();
    }

    Object* ObjectHolder::Get() const
    {
        return data_.get();
    }

    ObjectHolder::operator bool() const
    {
        return Get() != nullptr;
    }

    //------------------ObjectHolder private-----------------------

    ObjectHolder::ObjectHolder(std::shared_ptr<Object> data)
        : data_(std::move(data)) {}

    void ObjectHolder::AssertIsValid() const
    {
        assert(data_ != nullptr);
    }

    //------------functoins-----------------------------

    bool IsTrue(const ObjectHolder& object)
    {
        return (!object ||
            object.TryAs<Bool>()->GetValue() ||
            object.TryAs<Number>()->GetValue() == 0 ||
            object.TryAs<String>()->GetValue().empty()) ?
            false : true;
    }

    //---------------------ClassInstance-------------------------------

    void ClassInstance::Print(std::ostream& os, Context& context)
    {
        if (this->HasMethod("__str__", 0))
        {
            this->Call("__str__", {}, context)->Print(os, context);
        }
        else
        {
            os << this;
        }
    }

    bool ClassInstance::HasMethod(const std::string& method, size_t argument_count) const
    {
        return (cls_.GetMethod(method) && cls_.GetMethod(method)->formal_params.size() == argument_count);
    }

    Closure& ClassInstance::Fields()
    {
        // Заглушка. Реализуйте метод самостоятельно
        throw std::logic_error("Not implemented"s);
    }

    const Closure& ClassInstance::Fields() const
    {
        // Заглушка. Реализуйте метод самостоятельно
        throw std::logic_error("Not implemented"s);
    }

    ClassInstance::ClassInstance(const Class& cls)
        : cls_(cls) {}

    ObjectHolder ClassInstance::Call(const std::string& method, const std::vector<ObjectHolder>& actual_args, Context& context)
    {     
        // Заглушка. Реализуйте метод самостоятельно.
        throw std::runtime_error("Not implemented"s);
    }

    //--------------------------Class------------------------------------

    Class::Class(std::string name, std::vector<Method> methods, const Class* parent)
    {
        // Реализуйте метод самостоятельно 
    }

    const Method* Class::GetMethod(const std::string& name) const
    {
        // Заглушка. Реализуйте метод самостоятельно
        return nullptr;
    }

    [[nodiscard]] inline const std::string& Class::GetName() const
    {
        // Заглушка. Реализуйте метод самостоятельно.
        throw std::runtime_error("Not implemented"s);
    }

    void Class::Print(std::ostream& os, Context& context)
    {
        // Заглушка. Реализуйте метод самостоятельно
    }

    //------------------------Bool---------------------------------

    void Bool::Print(std::ostream& os, [[maybe_unused]] Context& context)
    {
        os << (GetValue() ? "True"sv : "False"sv);
    }

    //------------------functions-----------------------------------------

    bool Equal(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        // Заглушка. Реализуйте функцию самостоятельно
        throw std::runtime_error("Cannot compare objects for equality"s);
    }

    bool Less(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        // Заглушка. Реализуйте функцию самостоятельно
        throw std::runtime_error("Cannot compare objects for less"s);
    }

    bool NotEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        // Заглушка. Реализуйте функцию самостоятельно
        throw std::runtime_error("Cannot compare objects for equality"s);
    }

    bool Greater(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        // Заглушка. Реализуйте функцию самостоятельно
        throw std::runtime_error("Cannot compare objects for equality"s);
    }

    bool LessOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        // Заглушка. Реализуйте функцию самостоятельно
        throw std::runtime_error("Cannot compare objects for equality"s);
    }

    bool GreaterOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        // Заглушка. Реализуйте функцию самостоятельно
        throw std::runtime_error("Cannot compare objects for equality"s);
    }

}  // namespace runtime