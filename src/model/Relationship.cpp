#include "Relationship.h"

//----------------------------------------------------------------------------------------------

Relationship::Relationship(
  QObject* parent
)
  : BasicElement(
    ElementType::Relationship,
    parent
  )
{
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

Relationship::Relationship(
  const QString& name,
  QObject* parent
)
  : BasicElement(
    ElementType::Relationship,
    parent
  )
{
  setName(name);
  setSize(preferredSize());
}

//----------------------------------------------------------------------------------------------

QSizeF Relationship::minimumSize() const
{
  return QSizeF(100, 60);
}

//----------------------------------------------------------------------------------------------

QSizeF Relationship::preferredSize() const
{
  return QSizeF(140, 80);
}

//----------------------------------------------------------------------------------------------

std::unique_ptr<BasicElement> Relationship::clone() const
{
  auto cloned = std::make_unique<Relationship>();

  QVariantMap data = serialize();
  cloned->deserialize(data);

  return std::move(cloned);
}

//----------------------------------------------------------------------------------------------

QString Relationship::typeDisplayName() const
{
  return "Relacionamento";
}

//----------------------------------------------------------------------------------------------

void Relationship::addEnd(
  RelationshipEnd* end
)
{
  m_ends.append(end);
}

//----------------------------------------------------------------------------------------------

void Relationship::removeEnd(
  RelationshipEnd* end
)
{
  m_ends.removeAll(end);
}

//----------------------------------------------------------------------------------------------

void Relationship::clearEnds()
{
  m_ends.clear();
}

//----------------------------------------------------------------------------------------------

RelationshipEnd* Relationship::findEndByEntityId(
  const QString& entityId
) const
{
  for(RelationshipEnd* end : m_ends){
    if(end->entityId() == entityId){
      return end;
    }
  }

  return nullptr;
}

//----------------------------------------------------------------------------------------------