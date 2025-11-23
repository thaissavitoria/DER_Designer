#include "model/AttributeType.h"
#include <QtCore/QStringList>
#include <QtCore/QDebug>

//----------------------------------------------------------------------------------------------

AttributeType::Type AttributeType::attributeTypeFromString(
    const QString& typeString
)
{
    if (typeString == "Atributo Derivado") {
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
      case Type::Derived:
          return "Atributo Derivado";
      case Type::Multivalued:
          return "Atributo Multivalorado";
      case Type::Composite:
          return "Atributo Composto";
      case Type::Normal:
      default:
        return "Atributo Simples";
    }
}

//----------------------------------------------------------------------------------------------

QStringList AttributeType::getAllAttributeTypeStrings()
{
    QStringList types;
    types << attributeTypeToString(Type::Normal);
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
    types << Type::Derived;
    types << Type::Multivalued;
    types << Type::Composite;
    return types;
}

//----------------------------------------------------------------------------------------------
