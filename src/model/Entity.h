#ifndef ENTITY_H
#define ENTITY_H

#include "BasicElement.h"

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

    virtual ~Entity() = default;

	QSizeF minimumSize() const override;

    QSizeF preferredSize() const override;

    std::unique_ptr<BasicElement> clone() const override;

    QString typeDisplayName() const override;
};

#endif // ENTITY_H