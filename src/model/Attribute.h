#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "BasicElement.h"
#include "AttributeType.h"

class Attribute : public BasicElement
{
    Q_OBJECT

public:
    explicit Attribute(
        QObject* parent = nullptr
    );

    explicit Attribute(
        const QString& name,
        QObject* parent = nullptr
    );

    explicit Attribute(
        const QString& name,
        AttributeType::Type type,
        QObject* parent = nullptr
    );

    virtual ~Attribute() = default;

    QSizeF minimumSize() const override;

    QSizeF preferredSize() const override;

    std::unique_ptr<BasicElement> clone() const override;

    QString typeDisplayName() const override;

    AttributeType::Type attributeType() const;

    void setAttributeType(
        AttributeType::Type type
    );

    QString attributeTypeString() const;

    bool isPrimaryKey() const;

    void setPrimaryKey(
        bool isPrimary
    );

    bool isNormalAttribute() const;
    bool isKeyAttribute() const;
    bool isDerivedAttribute() const;
    bool isMultivaluedAttribute() const;
    bool isCompositeAttribute() const;

signals:
    void attributeTypeChanged(
        AttributeType::Type newType
    );

    void primaryKeyChanged(
        bool isPrimary
    );

private:
    AttributeType::Type m_attributeType;
    bool m_isPrimaryKey;

    void updateFromAttributeType();
};

#endif // ATTRIBUTE_H