#ifndef RELATIONSHIPCONNECTIONGRAPHICSITEM_H
#define RELATIONSHIPCONNECTIONGRAPHICSITEM_H

#include "ConnectionGraphicsItem.h"

class RelationshipConnectionLine;

// -----------------------------------------------------------------------------------------------------

class RelationshipConnectionLineGraphicsItem : public ConnectionGraphicsItem
{
  Q_OBJECT

public:
  explicit RelationshipConnectionLineGraphicsItem(
    RelationshipConnectionLine* connection,
    QGraphicsItem* parent = nullptr
  );

  virtual ~RelationshipConnectionLineGraphicsItem() = default;

  void paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget = nullptr
  ) override;

  QRectF boundingRect() const override;

  QPainterPath shape() const override;

protected:
  void mousePressEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void mouseMoveEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void mouseReleaseEvent(
    QGraphicsSceneMouseEvent* event
  ) override;

  void hoverMoveEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

private:
  static constexpr qreal CARDINALITY_FONT_SIZE = 12.0;

  RelationshipConnectionLine* m_relationshipConnection;
  bool m_isDraggingCardinality;
  QPointF m_dragStartOffset;

  void paintDoubleLine(
    QPainter* painter,
    const QPointF& start,
    const QPointF& end
  );

  void paintCardinality(
    QPainter* painter
  );

  QRectF getCardinalityBoundingRect() const;

  bool isPointNearCardinality(
    const QPointF& point
  ) const;
};

#endif // RELATIONSHIPCONNECTIONGRAPHICSITEM_H