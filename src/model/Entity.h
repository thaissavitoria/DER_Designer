#ifndef ENTITY_H
#define ENTITY_H

#include "BasicElement.h"
#include "Attribute.h"

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

    void addAttribute(
        Attribute* attribute
    );

    bool removeAttribute(
      Attribute* attribute
    );

    QList<Attribute*> getAttributes();

    bool isWeakEntity() const;

    void setIsWeakEntity(
      const bool isWeak
    );

  private:
    QList<Attribute*> m_attributes;
    bool m_isWeakEntity = false;
};

#endif // ENTITY_H