#ifndef RELATIONSHIPRENDERER_H
#define RELATIONSHIPRENDERER_H

#include "ElementRenderer.h"
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QPen>

// -----------------------------------------------------------------------------------------------------

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