#ifndef CONNECTIONGRAPHICSITEM_H
#define CONNECTIONGRAPHICSITEM_H

#include <QtWidgets/QGraphicsItem>
#include <QtGui/QPainter>
#include <QtCore/QObject>

#include "model/ConnectionLine.h"
#include "model/ConnectionPoint.h"

/**
 * @brief Graphic item that represents a ConnectionLine in the QGraphicsView
 *
 * Updates the visual representation of ConnectionLines with improved algorithms
 * for orthogonal and bezier paths based on connection point directions
 */
class ConnectionGraphicsItem : public QObject, public QGraphicsItem
{
  Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
  explicit ConnectionGraphicsItem(
    ConnectionLine* connection,
    QGraphicsItem* parent = nullptr
  );

  virtual ~ConnectionGraphicsItem();

  QRectF boundingRect() const override;

  void paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget = nullptr
  ) override;

  QPainterPath shape() const override;

  ConnectionLine* connection() const { return m_connection; }

protected:
  void mousePressEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void hoverEnterEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

  void hoverLeaveEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

private slots:
  void updateFromConnection();

private:
  ConnectionLine* m_connection;
  bool m_isHovered;

  void connectToConnection();
  void disconnectFromConnection();

  QPainterPath createStraightPath() const;
  QPainterPath createOrthogonalPath() const;
  QPainterPath createBezierPath() const;

  void createSimpleOrthogonalPath(
    QPainterPath& path,
    const QPointF& startPos,
    const QPointF& endPos
  ) const;

  void createOrthogonalPathWithDirections(
    QPainterPath& path,
    const QPointF& startPos,
    const QPointF& endPos,
    ConnectionDirection startDirection,
    ConnectionDirection endDirection
  ) const;

  QPointF calculateExitPoint(
    const QPointF& position,
    ConnectionDirection direction,
    qreal distance
  ) const;

  QPointF calculateControlPointFromDirection(
    const QPointF& position,
    ConnectionDirection direction,
    qreal distance
  ) const;

  Q_DISABLE_COPY(ConnectionGraphicsItem)
};

#endif // CONNECTIONGRAPHICSITEM_H