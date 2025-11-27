#include "Attribute.h"

// -----------------------------------------------------------------------------------------------------

Attribute::Attribute(
  QObject* parent
)
  : BasicElement(
    ElementType::Attribute,
    parent
  ),
  m_attributeType(AttributeType::Type::Normal),
  m_isPrimaryKey(false)
{
  setSize(preferredSize());
}

// -----------------------------------------------------------------------------------------------------

Attribute::Attribute(
  const QString& name,
  QObject* parent
)
  : BasicElement(
    ElementType::Attribute,
    parent
  ),
  m_attributeType(AttributeType::Type::Normal),
  m_isPrimaryKey(false)
{
  setName(name);
  setSize(preferredSize());
}

// -----------------------------------------------------------------------------------------------------

Attribute::Attribute(
  const QString& name,
  AttributeType::Type type,
  QObject* parent
)
  : BasicElement(
    ElementType::Attribute,
    parent
  ),
  m_attributeType(type),
  m_isPrimaryKey(false)
{
  setName(name);
  setSize(preferredSize());
}

// -----------------------------------------------------------------------------------------------------

Attribute::~Attribute()
{
  for (Attribute* attribute : m_subAttributes) {
    attribute->setParent(nullptr);
  }
}

// -----------------------------------------------------------------------------------------------------

QSizeF Attribute::minimumSize() const
{
  return QSizeF(60, 30);
}

// -----------------------------------------------------------------------------------------------------

QSizeF Attribute::preferredSize() const
{
  return QSizeF(100, 50);
}

// -----------------------------------------------------------------------------------------------------

std::unique_ptr<BasicElement> Attribute::clone() const
{
  auto cloned = std::make_unique<Attribute>();

  QVariantMap data = serialize();
  cloned->deserialize(data);

  cloned->setAttributeType(m_attributeType);
  cloned->setPrimaryKey(m_isPrimaryKey);

  return std::move(cloned);
}

// -----------------------------------------------------------------------------------------------------

QString Attribute::typeDisplayName() const
{
  return AttributeType::attributeTypeToString(m_attributeType);
}

// -----------------------------------------------------------------------------------------------------

AttributeType::Type Attribute::attributeType() const
{
  return m_attributeType;
}

// -----------------------------------------------------------------------------------------------------

void Attribute::setAttributeType(
  AttributeType::Type type
)
{
  if (m_attributeType != type) {
    if (m_attributeType == AttributeType::Composite && type != AttributeType::Composite) {
      QList<Attribute*> subAttributesToRemove = m_subAttributes;
      m_subAttributes.clear();

      for (auto subAttr : subAttributesToRemove) {
        emit subAttributeRemoved(subAttr);
      }
    }

    m_attributeType = type;
    emit attributeTypeChanged(type);
    emit elementChanged();
  }
}

// -----------------------------------------------------------------------------------------------------

QString Attribute::attributeTypeString() const
{
  return AttributeType::attributeTypeToString(m_attributeType);
}

// -----------------------------------------------------------------------------------------------------

bool Attribute::isPrimaryKey() const
{
  return m_isPrimaryKey;
}

// -----------------------------------------------------------------------------------------------------

void Attribute::setPrimaryKey(
  bool isPrimary
)
{
  if (m_isPrimaryKey != isPrimary) {
    m_isPrimaryKey = isPrimary;
    emit primaryKeyChanged(isPrimary);
    emit elementChanged();
  }
}

// -----------------------------------------------------------------------------------------------------

bool Attribute::isNormalAttribute() const
{
  return m_attributeType == AttributeType::Normal;
}

// -----------------------------------------------------------------------------------------------------

bool Attribute::isDerivedAttribute() const
{
  return m_attributeType == AttributeType::Derived;
}

// -----------------------------------------------------------------------------------------------------

bool Attribute::isMultivaluedAttribute() const
{
  return m_attributeType == AttributeType::Multivalued;
}

// -----------------------------------------------------------------------------------------------------

bool Attribute::isCompositeAttribute() const
{
  return m_attributeType == AttributeType::Composite;
}

// -----------------------------------------------------------------------------------------------------

void Attribute::addSubAttribute(
  Attribute* subAttribute
)
{
  if (subAttribute && !m_subAttributes.contains(subAttribute)) {
    m_subAttributes.append(subAttribute);
  }
}

// -----------------------------------------------------------------------------------------------------

bool Attribute::removeSubAttribute(
  Attribute* subAttribute
)
{
  bool removed = m_subAttributes.removeOne(subAttribute);
  if (removed) {
    subAttribute->setParent(nullptr);
  }
  return removed;
}

// -----------------------------------------------------------------------------------------------------

QList<Attribute*> Attribute::getSubAttributes() const
{
  return m_subAttributes;
}

// -----------------------------------------------------------------------------------------------------