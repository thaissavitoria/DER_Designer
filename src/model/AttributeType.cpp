#include "model/AttributeType.h"
#include <QtCore/QStringList>
#include <QtCore/QDebug>

//----------------------------------------------------------------------------------------------

AttributeType::Type AttributeType::attributeTypeFromString(
    const QString& typeString
)
{
    if (typeString == "Atributo Chave") {
        return Type::Key;
    }
    else if (typeString == "Atributo Derivado") {
        return Type::Derived;
    }
    else if (typeString == "Atributo Multivalorado") {
        return Type::Multivalued;
    }
    else if (typeString == "Atributo Composto") {
        return Type::Composite;
    }

    return Type::Normal;

}

//----------------------------------------------------------------------------------------------

QString AttributeType::attributeTypeToString(
    Type type
)
{
    switch (type) {
    case Type::Normal:
        return "Atributo Simples";
    case Type::Key:
        return "Atributo Chave";
    case Type::Derived:
        return "Atributo Derivado";
    case Type::Multivalued:
        return "Atributo Multivalorado";
    case Type::Composite:
        return "Atributo Composto";
    default:
        return "";
    }
}

//----------------------------------------------------------------------------------------------

QStringList AttributeType::getAllAttributeTypeStrings()
{
    QStringList types;
    types << attributeTypeToString(Type::Normal);
    types << attributeTypeToString(Type::Key);
    types << attributeTypeToString(Type::Derived);
    types << attributeTypeToString(Type::Multivalued);
    types << attributeTypeToString(Type::Composite);
    return types;
}

//----------------------------------------------------------------------------------------------

QList<AttributeType::Type> AttributeType::getAllAttributeTypes()
{
    QList<Type> types;
    types << Type::Normal;
    types << Type::Key;
    types << Type::Derived;
    types << Type::Multivalued;
    types << Type::Composite;
    return types;
}

//----------------------------------------------------------------------------------------------
