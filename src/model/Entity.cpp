#include "Entity.h"

// -----------------------------------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------------------------------

QSizeF Entity::minimumSize() const
{
    return QSizeF(80, 40);
}

// -----------------------------------------------------------------------------------------------------

QSizeF Entity::preferredSize() const
{
    return QSizeF(120, 60);
}

// -----------------------------------------------------------------------------------------------------

std::unique_ptr<BasicElement> Entity::clone() const
{
    auto cloned = std::make_unique<Entity>();

    QVariantMap data = serialize();
    cloned->deserialize(data);

    return std::move(cloned);
}

// -----------------------------------------------------------------------------------------------------

QString Entity::typeDisplayName() const
{
    return "Entidade";
}

// -----------------------------------------------------------------------------------------------------

void Entity::addAttribute(
  Attribute* attribute
) 
{
  if (attribute && !m_attributes.contains(attribute)) {
    m_attributes.append(attribute);
  }
}

// -----------------------------------------------------------------------------------------------------

bool Entity::removeAttribute(
  Attribute* attribute
) 
{
  return m_attributes.removeOne(attribute);
}

// -----------------------------------------------------------------------------------------------------

QList<Attribute*> Entity::getAttributes() 
{
  return m_attributes;
}

// -----------------------------------------------------------------------------------------------------

bool Entity::isWeakEntity() const 
{
  return m_isWeakEntity;
}

// -----------------------------------------------------------------------------------------------------

void Entity::setIsWeakEntity(
  const bool isWeak
)
{
  m_isWeakEntity = isWeak;
}

// -----------------------------------------------------------------------------------------------------