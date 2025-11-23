#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include "BasicElement.h"
#include "RelationshipEnd.h"

#include <QtCore/QList>

// -----------------------------------------------------------------------------------------------------

class Relationship : public BasicElement
{
  Q_OBJECT

public:
  explicit Relationship(
    QObject* parent = nullptr
  );

  explicit Relationship(
    const QString& name,
    const bool isIdentifying = false,
    QObject* parent = nullptr
  );

  virtual ~Relationship() = default;

  QSizeF minimumSize() const override;

  QSizeF preferredSize() const override;

  std::unique_ptr<BasicElement> clone() const override;

  QString typeDisplayName() const override;

  QList<RelationshipEnd*> ends() const { return m_ends; }

  void addEnd(
    RelationshipEnd* end
  );

  void removeEnd(
    RelationshipEnd* end
  );

  void clearEnds();

  RelationshipEnd* findEndByEntityId(
    const QString& entityId
  ) const;

  bool isIdentifying() const { return m_isIdentifying; }

  void setIsIdentifying(
    const bool isIdentifying
  );

  QVariantMap serialize() const override;

  bool deserialize(
    const QVariantMap& data
  ) override;

private:
  bool m_isIdentifying;
  QList<RelationshipEnd*> m_ends;
};

#endif // RELATIONSHIP_H