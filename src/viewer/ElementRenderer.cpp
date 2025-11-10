#include "ElementRenderer.h"
#include "EntityRenderer.h"
#include "AttributeRenderer.h"
#include "RelationshipRenderer.h"
#include "model/BasicElement.h"

#include <memory>

//----------------------------------------------------------------------------------------------

std::unique_ptr<IElementRenderer> ElementRendererFactory::createRenderer(
  ElementType type
)
{
  switch (type) {
    case ElementType::Entity:
      return std::make_unique<EntityRenderer>();

    case ElementType::Attribute:
        return std::make_unique<AttributeRenderer>();

    case ElementType::Relationship:
      return std::make_unique<RelationshipRenderer>();

    default:
      return nullptr;
  }
}

//----------------------------------------------------------------------------------------------
