#ifndef PROPERTYCOMMAND_H
#define PROPERTYCOMMAND_H

#include <QtCore/QString>
#include <QtCore/QVariant>

class BasicElement;

class PropertyCommand
{
public:
    static bool setElementProperty(
        BasicElement* element,
        const QString& propertyName,
        const QVariant& value
    );

private:
    static bool validateProperty(
        BasicElement* element,
        const QString& propertyName,
        const QVariant& value
    );
};

#endif // PROPERTYCOMMAND_H