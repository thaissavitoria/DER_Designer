#ifndef BASICELEMENT_H
#define BASICELEMENT_H

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QString>
#include <QtCore/QSizeF>
#include <QtCore/QUuid>
#include <QtCore/QVariant>

#include <memory>

#include "ConnectionPoint.h"

// -----------------------------------------------------------------------------------------------------

enum class ElementType {
  Unknown = 0,
  Entity,
  WeakEntity,
  Attribute,
  KeyAttribute,
  DerivedAttribute,
  MultivaluedAttribute,
  Relationship,
  IdentifyingRelationship
};

// -----------------------------------------------------------------------------------------------------

class IElementObserver
{
public:
  virtual ~IElementObserver() = default;

  virtual void onElementChanged(
    const class BasicElement* element
  ) = 0;

  virtual void onElementPositionChanged(
    const class BasicElement* element,
    const QPointF& oldPosition,
    const QPointF& newPosition
  ) = 0;
};

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Abstract base class for all ERD elements (Model only)
 *
 * This class provides:
 * - Basic properties common to all elements
 * - Change notification system (Observer Pattern)
 * - Serialization/Deserialization – to facilitate conversion to JSON
 * - Connection points for line connecting mechanism
 */
class BasicElement : public QObject
{
  Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(ElementType type READ type CONSTANT)
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)

public:
  explicit BasicElement(
    ElementType type,
    QObject* parent = nullptr
  );

  explicit BasicElement(
    const BasicElement& otherElement
  );

  virtual ~BasicElement();

  QString id() const { return m_id; }
  QString name() const { return m_name; }
  ElementType type() const { return m_type; }
  QPointF position() const { return m_position; }
  QSizeF size() const { return m_size; }
  QRectF boundingRect() const { return QRectF(m_position, m_size); }
  QList<ConnectionPoint*> connectionPoints() const { return m_connectionPoints; }

  void setName(
    const QString& name
  );

  void setPosition(
    const QPointF& position
  );

  void setSize(
    const QSizeF& size
  );

  void setPosition(
    qreal x,
    qreal y
  )
  {
    setPosition(QPointF(x, y));
  }

  void setSize(
    qreal width,
    qreal height
  ) 
  {
    setSize(QSizeF(width, height));
  }

  void move(
    const QPointF& delta
  );

  void resize(
    const QSizeF& newSize
  ) 
  {
    setSize(newSize);
  }

  void addObserver(
    IElementObserver* observer
  );

  void removeObserver(
    IElementObserver* observer
  );

  void addConnectionPoint(
    ConnectionPoint* connectionPoint
  );

  ConnectionPoint* getConnectionPointByDirection(
    ConnectionDirection connectionDirection
  );

  void createDefaultConnectionPoints();

  virtual bool isValid() const;

  virtual QVariantMap serialize() const;
  virtual bool deserialize(
    const QVariantMap& data
  );

  virtual QSizeF minimumSize() const = 0;
  virtual QSizeF preferredSize() const = 0;
  virtual std::unique_ptr<BasicElement> clone() const = 0;
  virtual QString typeDisplayName() const = 0;

  static QString elementTypeToString(
    ElementType type
  );

  void addAttributeId(
    const QString& attributeId
  );

  bool removeAttributeId(
    const QString& attributeId
  );

  QList<QString> getAttributeIds() const;

  void clearAttributesIds();

protected:
  void notifyObservers();

  void notifyPositionChanged(
    const QPointF& oldPosition,
    const QPointF& newPosition
  );

signals:
  void positionChanged(
    const QPointF& newPosition
  );

private:
  QString m_id;
  QString m_name;
  ElementType m_type;
  QPointF m_position;
  QSizeF m_size;

  QList<ConnectionPoint*> m_connectionPoints;

  QList<IElementObserver*> m_observers;

  QList<QString> m_attributeIds;
};

#endif // BASICELEMENT_H