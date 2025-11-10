#ifndef CONNECTIONPOINT_H
#define CONNECTIONPOINT_H

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QString>
#include <QtCore/QUuid>

// -----------------------------------------------------------------------------------------------------

enum class ConnectionDirection {
    Top = 0,
    Bottom,
    Left,
    Right
};

// -----------------------------------------------------------------------------------------------------

class ConnectionPoint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QPointF relativePosition READ relativePosition WRITE setRelativePosition NOTIFY relativePositionChanged)
    Q_PROPERTY(ConnectionDirection direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged)

public:
    explicit ConnectionPoint(
        QObject* parent = nullptr
    );

    explicit ConnectionPoint(
        const QPointF& relativePosition,
        ConnectionDirection direction,
        QObject* parent = nullptr
    );

    virtual ~ConnectionPoint();

    QString id() const { return m_id; }
    QPointF relativePosition() const { return m_relativePosition; }
    ConnectionDirection direction() const { return m_direction; }
    bool isConnected() const { return m_isConnected; }

    void setRelativePosition(
        const QPointF& position
    );
    
    void setDirection(
        ConnectionDirection direction
    );

    QPointF absolutePosition(
        const QPointF& elementPosition,
        const QSizeF& elementSize
    ) const;

    void setConnected(
        bool connected
    );

    static QString directionToString(
        ConnectionDirection direction
    );
    
    static ConnectionDirection directionFromString(
        const QString& directionString
    );

    static QList<ConnectionPoint*> createDefaultConnectionPoints(
        QObject* parent = nullptr
    );

signals:
    void relativePositionChanged(
        const QPointF& newPosition
    );
    
    void directionChanged(
        ConnectionDirection newDirection
    );
    
    void connectionStateChanged(
        bool isConnected
    );

private:
    static void addConectionPoint(
        QList<ConnectionPoint*>& list,
        const QPointF& relativePosition,
        ConnectionDirection direction,
        QObject* parent
    );

    QString m_id;
    QPointF m_relativePosition;
    ConnectionDirection m_direction;
    bool m_isConnected;

    Q_DISABLE_COPY(ConnectionPoint)
};

#endif // CONNECTIONPOINT_H