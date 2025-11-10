#include "RelationshipRenderer.h"

#include "model/BasicElement.h"
#include "model/Relationship.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtCore/QRectF>
#include <QtCore/qmath.h>

// -----------------------------------------------------------------------------------------------------

RelationshipRenderer::RelationshipRenderer()
  : m_textMargin(6.0)
{
  setupDefaultStyle();
}

// -----------------------------------------------------------------------------------------------------

void RelationshipRenderer::setupDefaultStyle()
{
  m_borderPen = QPen(Qt::black, 2.0, Qt::SolidLine);

  m_fillBrush = QBrush(Qt::white);

  m_textPen = QPen(Qt::black);
  m_textFont = QFont("Arial", 10);
  m_textFont.setBold(false);

  m_selectionBrush = QBrush(QColor(0, 0, 255, 20));
  m_selectionPen = QPen(Qt::blue, 2.5, Qt::SolidLine);
}

// -----------------------------------------------------------------------------------------------------

void RelationshipRenderer::render(
  QPainter* painter,
  const BasicElement* element,
  const QRectF& rect
)
{
  if (!painter || !element) return;

  auto relationship = qobject_cast<const Relationship*>(element);
  if (!relationship) return;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  renderDiamond(painter, rect);

  painter->setPen(m_textPen);
  painter->setFont(m_textFont);

  QRectF textRect = getTextRect(rect);
  painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, element->name());

  painter->restore();
}

// -----------------------------------------------------------------------------------------------------

void RelationshipRenderer::renderDiamond(
  QPainter* painter,
  const QRectF& rect
)
{
  painter->setPen(m_borderPen);
  painter->setBrush(m_fillBrush);

  QPolygonF diamond = getDiamondPolygon(rect);
  painter->drawPolygon(diamond);
}

// -----------------------------------------------------------------------------------------------------

bool RelationshipRenderer::isInElementRect(
  const BasicElement* element,
  const QPointF& point
)
{
  if (!element) return false;

  QRectF rect = element->boundingRect();
  return isPointInDiamond(point, rect);
}

// -----------------------------------------------------------------------------------------------------

QRectF RelationshipRenderer::selectionRect(
  const BasicElement* element
)
{
  if (!element) return QRectF();

  QRectF rect = element->boundingRect();
  qreal margin = 2.0;

  return rect.adjusted(-margin, -margin, margin, margin);
}

// -----------------------------------------------------------------------------------------------------

void RelationshipRenderer::renderSelection(
  QPainter* painter,
  const BasicElement* element,
  const QRectF& rect
)
{
  if (!painter || !element) return;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  painter->setPen(m_selectionPen);
  painter->setBrush(m_selectionBrush);

  QPolygonF diamond = getDiamondPolygon(rect);
  painter->drawPolygon(diamond);

  painter->restore();
}

// -----------------------------------------------------------------------------------------------------

QRectF RelationshipRenderer::getTextRect(
  const QRectF& elementRect
) const
{
  return elementRect.adjusted(m_textMargin, m_textMargin, -m_textMargin, -m_textMargin);
}

// -----------------------------------------------------------------------------------------------------

bool RelationshipRenderer::isPointInDiamond(
  const QPointF& point,
  const QRectF& diamondRect
) const
{
  if (diamondRect.isEmpty()) return false;

  QPolygonF diamond = getDiamondPolygon(diamondRect);
  return diamond.containsPoint(point, Qt::OddEvenFill);
}

// -----------------------------------------------------------------------------------------------------

QPolygonF RelationshipRenderer::getDiamondPolygon(
  const QRectF& rect
) const
{
  qreal halfWidth = rect.width() / 2.0;
  qreal halfHeight = rect.height() / 2.0;

  QPointF topLeft = rect.topLeft();

  QPolygonF diamond;
  diamond << QPointF(topLeft.x() + halfWidth, topLeft.y())                    // Topo
    << QPointF(topLeft.x() + rect.width(), topLeft.y() + halfHeight)    // Direita
    << QPointF(topLeft.x() + halfWidth, topLeft.y() + rect.height())    // Baixo
    << QPointF(topLeft.x(), topLeft.y() + halfHeight);                  // Esquerda

  return diamond;
}

// -----------------------------------------------------------------------------------------------------