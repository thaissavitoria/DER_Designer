#include "ElementFactory.h"

#include "model/Attribute.h"
#include "model/Entity.h"
#include "model/Relationship.h"

//----------------------------------------------------------------------------------------------

BasicElement* ElementFactory::createElement(
  ElementType type,
  QObject* parent
)
{
  switch (type) {
  case ElementType::Entity:
    return new Entity("Entidade", parent);

  case ElementType::WeakEntity:
    return new Entity("Entidade Fraca", true /*isWeak*/, parent);

  case ElementType::Attribute:
  case ElementType::KeyAttribute:
  case ElementType::DerivedAttribute:
  case ElementType::MultivaluedAttribute:
    return new Attribute("Atributo", parent);

  case ElementType::Relationship:
    return new Relationship("Relacionamento", false /*isIdentifying*/, parent);

  case ElementType::IdentifyingRelationship:
    return new Relationship("Rel. Identificador", true /*isIdentifying*/, parent);

  default:
    return nullptr;
  }
}

//----------------------------------------------------------------------------------------------

BasicElement* ElementFactory::createElement(
  const QString& typeName,
  QObject* parent
)
{
  if (typeName == "Entity") {
    return new Entity("Entidade", parent);
  }
  if (typeName == "WeakEntity") {
    return new Entity("Entidade Fraca", true /*isWeak*/, parent);
  }
  if (typeName == "Attribute") {
    return new Attribute("Atributo", parent);
  }
  if (typeName == "Relationship") {
    return new Relationship("Relacionamento", false /*isIdentifying*/, parent);
  }
  if (typeName == "IdentifyingRelationship") {
    return new Relationship("Rel. Identificador", true /*isIdentifying*/, parent);
  }

  return nullptr;
}
