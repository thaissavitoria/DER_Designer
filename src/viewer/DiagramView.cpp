#include "DiagramView.h"
#include "controller/DiagramScene.h"

#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QApplication>

//----------------------------------------------------------------------------------------------

DiagramView::DiagramView(
    DiagramScene* scene,
    QWidget* parent
)
    : QGraphicsView(scene, parent)
    , m_currentZoom(DEFAULT_ZOOM)
{
    setAcceptDrops(true);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

//----------------------------------------------------------------------------------------------

void DiagramView::dragEnterEvent(
    QDragEnterEvent* event
)
{
    if (event->mimeData()->hasFormat("application/x-element-type")) {
        QString elementType = QString::fromUtf8(event->mimeData()->data("application/x-element-type"));
        
        if (isValidElementType(elementType)) {
            event->acceptProposedAction();
            return;
        }
    }
    
    QGraphicsView::dragEnterEvent(event);
}

//----------------------------------------------------------------------------------------------

void DiagramView::dragMoveEvent(
    QDragMoveEvent* event
)
{
    if (event->mimeData()->hasFormat("application/x-element-type")) {
        QString elementType = QString::fromUtf8(event->mimeData()->data("application/x-element-type"));
        
        if (isValidElementType(elementType)) {
            event->acceptProposedAction();
            return;
        }
    }
    
    QGraphicsView::dragMoveEvent(event);
}

//----------------------------------------------------------------------------------------------

void DiagramView::dropEvent(
    QDropEvent* event
)
{
    if (event->mimeData()->hasFormat("application/x-element-type")) {
        QString elementType = QString::fromUtf8(event->mimeData()->data("application/x-element-type"));
        
        if (isValidElementType(elementType)) {
            QPointF scenePos = mapToScene(event->pos());
            emit elementDropped(elementType, scenePos);
            event->acceptProposedAction();
            return;
        }
    }
    
    QGraphicsView::dropEvent(event);
}

//----------------------------------------------------------------------------------------------

void DiagramView::wheelEvent(
    QWheelEvent* event
)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        const qreal scaleFactor = (event->angleDelta().y() > 0) ? ZOOM_STEP : (1.0 / ZOOM_STEP);
        const QPointF centerPoint = mapToScene(event->position().toPoint());
        
        applyZoom(scaleFactor, centerPoint);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

//----------------------------------------------------------------------------------------------

void DiagramView::drawBackground(
    QPainter* painter,
    const QRectF& rect
)
{
    painter->fillRect(rect, QColor(255, 255, 255));

    if (m_gridVisible) {
        drawGrid(painter, rect);
    }
}

//----------------------------------------------------------------------------------------------

void DiagramView::drawGrid(
    QPainter* painter,
    const QRectF& rect
)
{
    painter->save();

    QPen gridPen(QColor(220, 220, 220, 200), 1, Qt::SolidLine);
    painter->setPen(gridPen);

    qreal left = static_cast<int>(rect.left() / GRID_SIZE) * GRID_SIZE;
    qreal top = static_cast<int>(rect.top() / GRID_SIZE) * GRID_SIZE;

    for (qreal x = left; x <= rect.right(); x += GRID_SIZE) {
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    for (qreal y = top; y <= rect.bottom(); y += GRID_SIZE) {
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }

    painter->restore();
}

//----------------------------------------------------------------------------------------------

void DiagramView::zoomIn()
{
    applyZoom(ZOOM_STEP);
}

//----------------------------------------------------------------------------------------------

void DiagramView::zoomOut()
{
    applyZoom(1.0 / ZOOM_STEP);
}

//----------------------------------------------------------------------------------------------

void DiagramView::resetZoom()
{
    resetTransform();
    m_currentZoom = DEFAULT_ZOOM;
}

//----------------------------------------------------------------------------------------------

void DiagramView::setZoomLevel(
    qreal zoomLevel
)
{
    if (zoomLevel < MIN_ZOOM || zoomLevel > MAX_ZOOM) {
        return;
    }
    
    resetTransform();
    scale(zoomLevel, zoomLevel);
    m_currentZoom = zoomLevel;
}

//----------------------------------------------------------------------------------------------

qreal DiagramView::getZoomLevel() const
{
    return m_currentZoom;
}

//----------------------------------------------------------------------------------------------

void DiagramView::applyZoom(
    qreal scaleFactor,
    const QPointF& centerPoint
)
{
    qreal newZoom = m_currentZoom * scaleFactor;
    
    if (newZoom < MIN_ZOOM || newZoom > MAX_ZOOM) {
        return;
    }
    
    if (!centerPoint.isNull()) {
        centerOn(centerPoint);
    }
    
    scale(scaleFactor, scaleFactor);
    m_currentZoom = newZoom;
}

//----------------------------------------------------------------------------------------------

bool DiagramView::isValidElementType(
    const QString& elementType
) const
{
    QStringList validTypes = {
        "Entity",
        "WeakEntity", 
        "Attribute",
        "KeyAttribute",
        "DerivedAttribute",
        "MultivaluedAttribute",
        "Relationship",
        "IdentifyingRelationship"
    };
    
    return validTypes.contains(elementType);
}

//----------------------------------------------------------------------------------------------