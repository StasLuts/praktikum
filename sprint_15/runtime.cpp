#include "runtime.h"

#include <cassert>
#include <optional>
#include <sstream>
#include <iostream>

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
        if (!object)
        {
            return false;
        }
        auto obj_pth_bl = object.TryAs<Bool>();
        if (obj_pth_bl)
        {
            return obj_pth_bl->GetValue();
        }
        auto obj_pth_str = object.TryAs<String>();
        if (obj_pth_str && !obj_pth_str->GetValue().empty())
        {
            return true;
        }
        auto obj_pth_num = object.TryAs<Number>();
        if (obj_pth_num && obj_pth_num->GetValue() != 0)
        {
            return true;
        }
        auto obj_pth_vo_bl = object.TryAs<ValueObject<bool>>();
        if (obj_pth_vo_bl)
        {
            return obj_pth_vo_bl->GetValue();
        }
        return false;
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
        return fields_;
    }

    const Closure& ClassInstance::Fields() const
    {
        return fields_;
    }

    ClassInstance::ClassInstance(const Class& cls)
        : cls_(cls) {}

    ObjectHolder ClassInstance::Call(const std::string& method, const std::vector<ObjectHolder>& actual_args, Context& context)
    {
        if (!this->HasMethod(method, actual_args.size()))
        {
            throw std::runtime_error("Not implemented"s);
        }
        Closure closure = { {"self", ObjectHolder::Share(*this)} };
        auto method_ptr = cls_.GetMethod(method);
        for (size_t i = 0; i < method_ptr->formal_params.size(); ++i)
        {
            auto arg = method_ptr->formal_params[i];
            closure[arg] = actual_args[i];
        }
        return method_ptr->body->Execute(closure, context);
    }

    //--------------------------Class------------------------------------

    Class::Class(std::string name, std::vector<Method> methods, const Class* parent)
        : name_(std::move(name)), methods_(std::move(methods)), parent_(std::move(parent))
    {
        if (parent_)
        {
            for (const auto& method : parent_->methods_)
            {
                names_methods_[method.name] = &method;
            }
        }
        for (const auto& method : methods_)
        {
            names_methods_[method.name] = &method;
        }
    }

    const Method* Class::GetMethod(const std::string& name) const
    {
        return (names_methods_.find(name) == names_methods_.end()) ? nullptr : names_methods_.at(name);
    }


    const std::string& Class::GetName() const
    {
        return name_;
    }

    void Class::Print(std::ostream& os, Context& /*context*/)
    {
        os << "Class "sv << GetName();
    }

    //------------------------Bool---------------------------------

    void Bool::Print(std::ostream& os, [[maybe_unused]] Context& /*context*/)
    {
        os << (GetValue() ? "True"sv : "False"sv);
    }

    //------------------functions-----------------------------------------

    bool Equal(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        if (!lhs && !rhs)
        {
            return true;
        }
        if (!lhs)
        {
            throw std::runtime_error("Cannot compare objects for equality"s);
        }
        auto lhs_ptr_str = lhs.TryAs<String>();
        auto rhs_ptr_str = rhs.TryAs<String>();
        if (lhs_ptr_str && rhs_ptr_str)
        {
            return lhs_ptr_str->GetValue() == rhs_ptr_str->GetValue();
        }
        auto lhs_ptr_num = lhs.TryAs<Number>();
        auto rhs_ptr_num = rhs.TryAs<Number>();
        if (lhs_ptr_num && rhs_ptr_num)
        {
            return lhs_ptr_num->GetValue() == rhs_ptr_num->GetValue();
        }
        auto lhs_ptr_bl = lhs.TryAs<Bool>();
        auto rhs_ptr_bl = rhs.TryAs<Bool>();
        if (lhs_ptr_bl && rhs_ptr_bl)
        {
            return lhs_ptr_bl->GetValue() == rhs_ptr_bl->GetValue();
        }
        auto lhs_ptr_cl = lhs.TryAs<ClassInstance>();
        if (lhs_ptr_cl && lhs_ptr_cl->HasMethod("__eq__", 1))
        {
            auto res = lhs_ptr_cl->Call("__eq__", { rhs }, context);
            return res.TryAs<Bool>()->GetValue();
        }
        throw std::runtime_error("Cannot compare objects for equality"s);
    }

    bool Less(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        if (!lhs)
        {
            throw std::runtime_error("Cannot compare objects for less"s);
        }
        auto lhs_ptr_str = lhs.TryAs<String>();
        auto rhs_ptr_str = rhs.TryAs<String>();
        if (lhs_ptr_str && rhs_ptr_str)
        {
            return lhs_ptr_str->GetValue() < rhs_ptr_str->GetValue();
        }
        auto lhs_ptr_num = lhs.TryAs<Number>();
        auto rhs_ptr_num = rhs.TryAs<Number>();
        if (lhs_ptr_num && rhs_ptr_num)
        {
            return lhs_ptr_num->GetValue() < rhs_ptr_num->GetValue();
        }
        auto lhs_ptr_bl = lhs.TryAs<Bool>();
        auto rhs_ptr_bl = rhs.TryAs<Bool>();
        if (lhs_ptr_bl && rhs_ptr_bl)
        {
            return lhs_ptr_bl->GetValue() < rhs_ptr_bl->GetValue();
        }
        auto lhs_ptr_cl = lhs.TryAs<ClassInstance>();
        if (lhs_ptr_cl && lhs_ptr_cl->HasMethod("__lt__", 1))
        {
            auto res = lhs_ptr_cl->Call("__lt__", { rhs }, context);
            return res.TryAs<Bool>()->GetValue();
        }
        throw std::runtime_error("Cannot compare objects for less"s);
    }

    bool NotEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        return !Equal(lhs, rhs, context);
    }

    bool Greater(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        return !Less(lhs, rhs, context) && !Equal(lhs, rhs, context);
    }

    bool LessOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        return Less(lhs, rhs, context) || Equal(lhs, rhs, context);
    }

    bool GreaterOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, Context& context)
    {
        return !Less(lhs, rhs, context);
    }

}  // namespace runtime