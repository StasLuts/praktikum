#include "statement.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace ast
{

    using runtime::Closure;
    using runtime::Context;
    using runtime::ObjectHolder;

    namespace
    {
        const string ADD_METHOD = "__add__"s;
        const string INIT_METHOD = "__init__"s;

    }  // namespace

    //-------------------------------Assignment-----------------------------------

    ObjectHolder Assignment::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    Assignment::Assignment(std::string /*var*/, std::unique_ptr<Statement> /*rv*/)
    {

    }

    //-------------------------VariableValue----------------------------------------

    VariableValue::VariableValue(const std::string& /*var_name*/)
    {

    }

    VariableValue::VariableValue(std::vector<std::string> /*dotted_ids*/)
    {

    }

    ObjectHolder VariableValue::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //-------------------------Print------------------------------------------------

    unique_ptr<Print> Print::Variable(const std::string& /*name*/)
    {
        // Заглушка, реализуйте метод самостоятельно
        throw std::logic_error("Not implemented"s);
    }

    Print::Print(unique_ptr<Statement> /*argument*/)
    {
        // Заглушка, реализуйте метод самостоятельно
    }

    Print::Print(vector<unique_ptr<Statement>> /*args*/)
    {
        // Заглушка, реализуйте метод самостоятельно
    }

    ObjectHolder Print::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //----------------------------MethodCall---------------------------------------

    MethodCall::MethodCall(std::unique_ptr<Statement> /*object*/, std::string /*method*/, std::vector<std::unique_ptr<Statement>> /*args*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
    }

    ObjectHolder MethodCall::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //----------------------Stringify-------------------------------------------

    ObjectHolder Stringify::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //-------------------------------------Add-------------------------------

    ObjectHolder Add::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //-------------------------Sub---------------------------------------

    ObjectHolder Sub::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //---------------------------Mult---------------------------------------

    ObjectHolder Mult::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //---------------------------------Div---------------------------------

    ObjectHolder Div::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //---------------------------Compound------------------------------------

    ObjectHolder Compound::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //-----------------------------Return-----------------------------------

    ObjectHolder Return::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //------------------------ClassDefinition--------------------------------

    ClassDefinition::ClassDefinition(ObjectHolder /*cls*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
    }

    ObjectHolder ClassDefinition::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //-------------------------FieldAssignment----------------------------------------

    FieldAssignment::FieldAssignment(VariableValue /*object*/, std::string /*field_name*/, std::unique_ptr<Statement> /*rv*/)
    {
    }

    ObjectHolder FieldAssignment::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //--------------------------------IfElse-----------------------------------------------------

    IfElse::IfElse(std::unique_ptr<Statement> /*condition*/, std::unique_ptr<Statement> /*if_body*/, std::unique_ptr<Statement> /*else_body*/)
    {
        // Реализуйте метод самостоятельно
    }

    ObjectHolder IfElse::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //--------------------------------Or----------------------------------

    ObjectHolder Or::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //-----------------------------And----------------------------------

    ObjectHolder And::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //----------------------------Not-----------------------------------

    ObjectHolder Not::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //-------------------------------Comparison-----------------------------------------------

    Comparison::Comparison(Comparator /*cmp*/, unique_ptr<Statement> lhs, unique_ptr<Statement> rhs)
    : BinaryOperation(std::move(lhs), std::move(rhs))
    {
        // Реализуйте метод самостоятельно
    }

    ObjectHolder Comparison::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

    //------------------------------NewInstance-----------------------------------------

    NewInstance::NewInstance(const runtime::Class& /*class_*/, std::vector<std::unique_ptr<Statement>> /*args*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
    }

    NewInstance::NewInstance(const runtime::Class& /*class_*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
    }

    ObjectHolder NewInstance::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }
    
    //-----------------------MethodBody--------------------------

    MethodBody::MethodBody(std::unique_ptr<Statement>&& /*body*/)
    {
    }

    ObjectHolder MethodBody::Execute(Closure& /*closure*/, Context& /*context*/)
    {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }

}  // namespace ast