#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include "BasicElement.h"

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
    QObject* parent = nullptr
  );

  virtual ~Relationship() = default;

  QSizeF minimumSize() const override;

  QSizeF preferredSize() const override;

  std::unique_ptr<BasicElement> clone() const override;

  QString typeDisplayName() const override;
};

#endif // RELATIONSHIP_H