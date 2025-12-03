#ifndef ENTITY_H
#define ENTITY_H

#include "BasicElement.h"
#include "Attribute.h"

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

  virtual ~Entity() = default;

  QSizeF minimumSize() const override;

  QSizeF preferredSize() const override;

  std::unique_ptr<BasicElement> clone() const override;

  QString typeDisplayName() const override;

  void addAttributeId(
    const QString& attributeId
  );

  bool removeAttributeId(
    const QString& attributeId
  );

  QList<QString> getAttributeIds() const;

  bool isWeakEntity() const;

  void setIsWeakEntity(
    const bool isWeak
  );

  QVariantMap serialize() const;
  bool deserialize(
    const QVariantMap& data
  );

private:
  QList<QString> m_attributeIds; 
  bool m_isWeakEntity = false;
};

#endif // ENTITY_H