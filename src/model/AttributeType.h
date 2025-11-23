#ifndef ATTRIBUTETYPE_H
#define ATTRIBUTETYPE_H

#include <QtCore/QString>
#include <QtCore/QStringList>

//----------------------------------------------------------------------------------------------

class AttributeType {
public:
    enum Type {
        Normal = 0,
        Derived,
        Multivalued,
        Composite
    };

    static Type attributeTypeFromString(
        const QString& typeString
    );

    static QString attributeTypeToString(
        Type type
    );
    
    static QStringList getAllAttributeTypeStrings();

    static QList<Type> getAllAttributeTypes();

};

#endif // ATTRIBUTETYPE_H