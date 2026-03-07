#ifndef ENTITYRENDERER_H
#define ENTITYRENDERER_H

#include "ElementRenderer.h"
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QPen>

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Renderer implementation for entity elements in ERD diagrams
 *
 * This class implements the IElementRenderer interface to provide specialized
 * rendering for entity elements following Peter Chen's ERD notation. It supports:
 * - Regular entity representation (single rectangle)
 * - Weak entity representation (double rectangle with inner margin)
 * - Selection visualization
 * - Hit detection for user interaction
 * - Customizable fonts, pens, and brushes for visual styling
 */

class EntityRenderer : public IElementRenderer
{
public:
  EntityRenderer();

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

  QRectF getTextRect(
    const QRectF& elementRect
  ) const;
};

#endif // ENTITYRENDERER_H