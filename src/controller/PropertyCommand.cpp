#include "PropertyCommand.h"
#include "model/BasicElement.h"
#include "model/Attribute.h"
#include "model/AttributeType.h"

//----------------------------------------------------------------------------------------------

bool PropertyCommand::setElementProperty(
    BasicElement* element,
    const QString& propertyName,
    const QVariant& value
)
{
    if (!element || !validateProperty(element, propertyName, value)) {
        return false;
    }

    if (propertyName == "name") {
        element->setName(value.toString());
        return true;
    }
    else if (propertyName == "posX") {
        QPointF pos = element->position();
        pos.setX(value.toReal());
        element->setPosition(pos);
        return true;
    }
    else if (propertyName == "posY") {
        QPointF pos = element->position();
        pos.setY(value.toReal());
        element->setPosition(pos);
        return true;
    }
    else if (propertyName == "width") {
        QSizeF size = element->size();
        size.setWidth(value.toReal());
        element->setSize(size);
        return true;
    }
    else if (propertyName == "height") {
        QSizeF size = element->size();
        size.setHeight(value.toReal());
        element->setSize(size);
        return true;
    }
    else if (propertyName == "attributeType") {
        auto attribute = qobject_cast<Attribute*>(element);
        if (attribute) {
            QString typeString = value.toString();
            AttributeType::Type newType = AttributeType::attributeTypeFromString(typeString);
            attribute->setAttributeType(newType);
            return true;
        }
        return false;
    }
    else if (propertyName == "isPrimaryKey") {
        auto attribute = qobject_cast<Attribute*>(element);
        if (attribute) {
            attribute->setPrimaryKey(value.toBool());
            return true;
        }
        return false;
    }
    else if (propertyName.startsWith("custom:")) {
        QString customKey = propertyName.mid(7);
        element->setProperty(customKey, value);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------

bool PropertyCommand::validateProperty(
    BasicElement* element,
    const QString& propertyName,
    const QVariant& value
)
{
    if (!element) {
        return false;
    }

    if (propertyName.trimmed().isEmpty()) {
        return false;
    }

    if (propertyName == "name") {
        return !value.toString().trimmed().isEmpty();
    }
    else if (propertyName == "width" || propertyName == "height") {
        bool ok;
        qreal numericValue = value.toString().toDouble(&ok);
        return ok && numericValue > 0;
    }
    else if (propertyName == "posX" || propertyName == "posY") {
        bool ok;
        value.toString().toDouble(&ok);
        return ok;
    }
    else if (propertyName == "attributeType") {
        QString typeString = value.toString().trimmed();
        if (typeString.isEmpty()) return false;

        QStringList validTypes = AttributeType::getAllAttributeTypeStrings();
        return validTypes.contains(typeString);
    }
    else if (propertyName == "isPrimaryKey" || propertyName == "isRequired") {
        if (value.type() == QVariant::Bool) {
            return true;
        }
        QString stringValue = value.toString().toLower().trimmed();
        return stringValue == "true" || stringValue == "false";
    }
    else if (propertyName.startsWith("custom:")) {
        return !propertyName.mid(7).trimmed().isEmpty();
    }

    return true;
}