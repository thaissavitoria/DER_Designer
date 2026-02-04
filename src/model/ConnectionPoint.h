#ifndef CONNECTIONPOINT_H
#define CONNECTIONPOINT_H

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QString>
#include <QtCore/QUuid>

// -----------------------------------------------------------------------------------------------------
/**
 * @brief Enumeration for connection point directions on element boundaries
 *
 * Defines the four cardinal directions where connection points can be placed
 * on ERD element boundaries
 */
enum class ConnectionDirection {
  Top = 0,
  Bottom,
  Left,
  Right
};

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Represents a connection point on an ERD element for line attachment
 *
 * This class manages connection points that serve as attachment locations for
 * connection lines between ERD elements. It provides:
 * - Unique identification through UUID for tracking connections
 * - Relative positioning within element boundaries (0.0 to 1.0 coordinates)
 * - Direction specification for line orientation (Top, Bottom, Left, Right)
 * - Absolute position calculation based on element position and size
 * - Factory method for creating default connection points (4 cardinal directions)
 * - Change notification system through Qt signals
 */
class ConnectionPoint : public QObject
{
  Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QPointF relativePosition READ relativePosition WRITE setRelativePosition NOTIFY relativePositionChanged)
    Q_PROPERTY(ConnectionDirection direction READ direction)

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

  void setRelativePosition(
    const QPointF& position
  );

  QPointF absolutePosition(
    const QPointF& elementPosition,
    const QSizeF& elementSize
  ) const;

  static QList<ConnectionPoint*> createDefaultConnectionPoints();

signals:
  void relativePositionChanged(
    const QPointF& newPosition
  );

private:
  static void addConnectionPoint(
    QList<ConnectionPoint*>& list,
    const QPointF& relativePosition,
    ConnectionDirection direction
  );

  QString m_id;
  QPointF m_relativePosition;
  ConnectionDirection m_direction;

  Q_DISABLE_COPY(ConnectionPoint)
};

#endif // CONNECTIONPOINT_H