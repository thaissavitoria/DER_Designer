#include "Entity.h"

//----------------------------------------------------------------------------------------------

Entity::Entity(
  QObject* parent
)
  : BasicElement(
    ElementType::Entity,
    parent
  )
{
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Entity::Entity(
  const QString& name,
  QObject* parent
)
  : BasicElement(
    ElementType::Entity,
    parent
  )
{
  setName(name);
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Entity::Entity(
  const QString& name,
  const bool isWeak,
  QObject* parent
)
  : BasicElement(
    ElementType::Entity,
    parent
  )
{
  setName(name);
  setIsWeakEntity(isWeak);
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

QSizeF Entity::minimumSize() const
{
  return QSizeF(80, 40);
}

//----------------------------------------------------------------------------------------------

QSizeF Entity::preferredSize() const
{
  return QSizeF(120, 60);
}

//----------------------------------------------------------------------------------------------

std::unique_ptr<BasicElement> Entity::clone() const
{
  auto cloned = std::make_unique<Entity>();

  QVariantMap data = serialize();
  cloned->deserialize(data);

  return std::move(cloned);
}

//----------------------------------------------------------------------------------------------

QString Entity::typeDisplayName() const
{
  return "Entidade";
}

//----------------------------------------------------------------------------------------------

void Entity::addAttributeId(
  const QString& attributeId
)
{
  if (!attributeId.isEmpty() && !m_attributeIds.contains(attributeId)) {
    m_attributeIds.append(attributeId);
  }
}

//----------------------------------------------------------------------------------------------

bool Entity::removeAttributeId(
  const QString& attributeId
)
{
  return m_attributeIds.removeOne(attributeId);
}

//----------------------------------------------------------------------------------------------

QList<QString> Entity::getAttributeIds() const
{
  return m_attributeIds;
}

//----------------------------------------------------------------------------------------------

bool Entity::isWeakEntity() const
{
  return m_isWeakEntity;
}

//----------------------------------------------------------------------------------------------

void Entity::setIsWeakEntity(
  const bool isWeak
)
{
  m_isWeakEntity = isWeak;
}

//----------------------------------------------------------------------------------------------

QVariantMap Entity::serialize() const
{
  QVariantMap data = BasicElement::serialize();

  data["isWeakEntity"] = m_isWeakEntity;

  if (!m_attributeIds.isEmpty()) {
    QVariantList attributesList;
    for (const QString& attrId : m_attributeIds) {
      attributesList.append(attrId);
    }
    data["attributeIds"] = attributesList;
  }

  return data;
}

//----------------------------------------------------------------------------------------------

bool Entity::deserialize(
  const QVariantMap& data
)
{
  if (!BasicElement::deserialize(data)) {
    return false;
  }

  try {
    m_isWeakEntity = data["isWeakEntity"].toBool();

    m_attributeIds.clear();

    if (data.contains("attributeIds")) {
      QVariantList attributesList = data["attributeIds"].toList();
      for (const QVariant& attrVariant : attributesList) {
        QString attrId = attrVariant.toString();
        if (!attrId.isEmpty()) {
          m_attributeIds.append(attrId);
        }
      }
    }

    return true;
  }
  catch (...) {
    qWarning() << "Erro ao deserializar entity" << id();
    return false;
  }
}

//----------------------------------------------------------------------------------------------