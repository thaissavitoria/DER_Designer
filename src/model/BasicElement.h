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
 * This class implements the MVC pattern as the Model, providing:
 * - Basic properties common to all elements
 * - Change notification system (Observer Pattern)
 * - Data validation
 * - Serialization/Deserialization – to facilitate conversion to JSON
 * - Connection points for line connecting mechanism
 */
class BasicElement : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QString id READ id CONSTANT)
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(ElementType type READ type CONSTANT)
        Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QSizeF size READ size WRITE setSize NOTIFY sizeChanged)

public:
    explicit BasicElement(
        ElementType type,
        QObject* parent = nullptr
    );

    virtual ~BasicElement();

    QString id() const { return m_id; }
    QString name() const { return m_name; }
    ElementType type() const { return m_type; }
    QPointF position() const { return m_position; }
    QSizeF size() const { return m_size; }
    QRectF boundingRect() const { return QRectF(m_position, m_size); }

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
    ) {
        setPosition(QPointF(x, y));
    }
    void setSize(
        qreal width,
        qreal height
    ) {
        setSize(QSizeF(width, height));
    }
    void move(
        const QPointF& delta
    );
    void resize(
        const QSizeF& newSize
    ) {
        setSize(newSize);
    }

    void setProperty(
        const QString& key,
        const QVariant& value
    );

    QVariant getProperty(
        const QString& key,
        const QVariant& defaultValue = QVariant()
    ) const;

    bool hasProperty(
        const QString& key
    ) const;

    void removeProperty(
        const QString& key
    );

    QStringList propertyKeys() const;

    void addObserver(
        IElementObserver* observer
    );
    void removeObserver(
        IElementObserver* observer
    );

    QList<ConnectionPoint*> connectionPoints() const { return m_connectionPoints; }
    
    void addConnectionPoint(
        ConnectionPoint* connectionPoint
    );
    
    void createDefaultConnectionPoints();

    virtual bool isValid() const;
    virtual QStringList validationErrors() const;

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
    static ElementType elementTypeFromString(
        const QString& typeString
    );

protected:
    void notifyObservers();
    void notifyPositionChanged(
        const QPointF& oldPosition,
        const QPointF& newPosition
    );
    virtual void onNameChanged(
        const QString& oldName,
        const QString& newName
    );
    virtual void onPositionChanged(
        const QPointF& oldPosition,
        const QPointF& newPosition
    );
    virtual void onSizeChanged(
        const QSizeF& oldSize,
        const QSizeF& newSize
    );

signals:
    void nameChanged(
        const QString& newName
    );
    void positionChanged(
        const QPointF& newPosition
    );
    void sizeChanged(
        const QSizeF& newSize
    );
    void propertyChanged(
        const QString& key,
        const QVariant& value
    );
    void elementChanged();

private:
    QString m_id;
    QString m_name;
    ElementType m_type;
    QPointF m_position;
    QSizeF m_size;

    QHash<QString, QVariant> m_customProperties;
    QList<ConnectionPoint*> m_connectionPoints;

    QList<IElementObserver*> m_observers;
    Q_DISABLE_COPY(BasicElement)
};

#endif // BASICELEMENT_H