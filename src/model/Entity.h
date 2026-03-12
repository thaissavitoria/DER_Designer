#ifndef ENTITY_H
#define ENTITY_H

#include "BasicElement.h"
#include "Attribute.h"

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Represents an entity element in an ERD diagram
 *
 * This class extends BasicElement to represent entities in Entity-Relationship diagrams
 * following Peter Chen's notation. It provides:
 * - Support for associated attributes through attribute IDs (from BasicElement)
 * - Serialization for persistence
 * - Type identification for rendering and behavior
 */

class Entity : public BasicElement
{
  Q_OBJECT

public:
  explicit Entity(
    QObject* parent = nullptr
  );

  explicit Entity(
    const QString& name,
    QObject* parent = nullptr
  );

  explicit Entity(
    const QString& name,
    const bool isWeak,
    QObject* parent = nullptr
  );

  explicit Entity(
    const Entity& otherEntity
  );

  virtual ~Entity() = default;

  QSizeF minimumSize() const override;

  QSizeF preferredSize() const override;

  std::unique_ptr<BasicElement> clone() const override;

  QString typeDisplayName() const override;

  bool isWeakEntity() const;

  QVariantMap serialize() const;
  bool deserialize(
    const QVariantMap& data
  );

  QString stringElementType() const override;

private:
  bool m_isWeakEntity = false;
};

#endif // ENTITY_H