#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "BasicElement.h"
#include "AttributeType.h"

/**
 * @brief Represents an attribute element in an ERD diagram
 *
 * This class extends BasicElement to represent attributes in Entity-Relationship diagrams
 * following Peter Chen's notation. It provides:
 * - Different attribute types (normal, derived, multivalued, composite)
 * - Primary key designation
 * - Support for composite attributes with sub-attributes
 * - Type-specific rendering properties
 * - Serialization for persistence
 */

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

  explicit Attribute(
    const Attribute& otherAtrribute
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
  bool isDerivedAttribute() const;
  bool isMultivaluedAttribute() const;
  bool isCompositeAttribute() const;

  QVariantMap serialize() const override;
  bool deserialize(
    const QVariantMap& data
  ) override;

private:
  AttributeType::Type m_attributeType;
  bool m_isPrimaryKey;
};

#endif // ATTRIBUTE_H