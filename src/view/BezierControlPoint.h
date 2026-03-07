#ifndef BEZIERCONTROLPOINT_H
#define BEZIERCONTROLPOINT_H

#include <QtWidgets/QGraphicsEllipseItem>
#include <QtGui/QPen>
#include <QtGui/QBrush>

class ConnectionGraphicsItem;

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Enumeration for Bezier control point types
 *
 * Identifies which control point is being manipulated in a Bezier curve:
 */
enum class ControlPointType {
  Control1,
  Control2
};

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Interactive control point for Bezier curve manipulation in connections
 *
 * This class provides a visual and interactive control point for adjusting Bezier
 * curves in connection lines. It extends QGraphicsEllipseItem to provide:
 * - Visual representation as a draggable ellipse handle
 * - Mouse interaction for dragging and repositioning
 * - Hover effects for visual feedback
 * - Communication with parent ConnectionGraphicsItem to update curve shape
 * - Type identification (Control1 or Control2) for proper curve calculation
 */

class BezierControlPoint : public QGraphicsEllipseItem
{
public:
  explicit BezierControlPoint(
    ConnectionGraphicsItem* parent,
    ControlPointType type
  );

  void updatePosition(
    const QPointF& position
  );

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

  void hoverEnterEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

  void hoverLeaveEvent(
    QGraphicsSceneHoverEvent* event
  ) override;

private:
  ConnectionGraphicsItem* m_connectionItem;
  ControlPointType m_type;
  bool m_isDragging;
};

#endif // BEZIERCONTROLPOINT_H