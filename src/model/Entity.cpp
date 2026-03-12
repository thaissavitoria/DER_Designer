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
  ),
  m_isWeakEntity(isWeak)
{
  setName(name);
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Entity::Entity(
  const Entity& otherEntity
)
  : BasicElement(otherEntity),
  m_isWeakEntity(otherEntity.m_isWeakEntity)
{
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
  auto cloned = std::make_unique<Entity>(*this);

  return cloned;
}

//----------------------------------------------------------------------------------------------

QString Entity::typeDisplayName() const
{
  return "Entidade";
}

//----------------------------------------------------------------------------------------------

bool Entity::isWeakEntity() const
{
  return m_isWeakEntity;
}

//----------------------------------------------------------------------------------------------

QVariantMap Entity::serialize() const
{
  QVariantMap data = BasicElement::serialize();

  data["isWeakEntity"] = m_isWeakEntity;

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
    return true;
  }
  catch (...) {
    return false;
  }
}

//----------------------------------------------------------------------------------------------

QString Entity::stringElementType() const
{
  return m_isWeakEntity? "Entidade Fraca": "Entidade";
}

//----------------------------------------------------------------------------------------------
