#ifndef RELATIONSHIPRENDERER_H
#define RELATIONSHIPRENDERER_H

#include "ElementRenderer.h"
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QPen>

// -----------------------------------------------------------------------------------------------------
/**
 * @brief Renderer implementation for relationship elements in ERD diagrams
 *
 * This class implements the IElementRenderer interface to provide specialized
 * rendering for relationship elements following Peter Chen's ERD notation. It supports:
 * - Regular relationship representation (single diamond)
 * - Identifying relationship representation (double diamond with inner margin)
 * - Selection visualization
 * - Customizable fonts, pens, and brushes for visual styling
 *
 * The diamond shape is rendered using polygon geometry with four points (top, right,
 * bottom, left) calculated from the element's bounding rectangle, providing the
 * distinctive appearance of relationships in ER diagrams.
 */
class RelationshipRenderer : public IElementRenderer
{
public:
  RelationshipRenderer();

  void render(
    QPainter* painter,
    const BasicElement* element,
    const QRectF& rect
  ) override;

  bool isInElementRect(
    const BasicElement* element,
    const QPointF& point
  ) override;

  QRectF selectionRect(
    const BasicElement* element
  ) override;

  void renderSelection(
    QPainter* painter,
    const BasicElement* element,
    const QRectF& rect
  ) override;

private:
  QPen m_borderPen;
  QBrush m_fillBrush;
  QBrush m_selectionBrush;
  QFont m_textFont;
  QPen m_textPen;
  QPen m_selectionPen;

  qreal m_textMargin;

  void setupDefaultStyle();

  void renderDiamond(
    QPainter* painter,
    const QRectF& rect,
    const bool isIdentifying
  );

  QRectF getTextRect(
    const QRectF& elementRect
  ) const;

  bool isPointInDiamond(
    const QPointF& point,
    const QRectF& diamondRect
  ) const;

  QPolygonF getDiamondPolygon(
    const QRectF& rect
  ) const;
};

#endif // RELATIONSHIPRENDERER_H