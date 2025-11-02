#include "DiagramView.h"
#include "DiagramScene.h"
#include <QtWidgets/QApplication>
#include <QtGui/QWheelEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QScrollBar>
#include <QtCore/qmath.h>
#include <QtCore/QDebug>

//-----------------------------------------------------------

DiagramView::DiagramView(
    QWidget* parent
)
    : QGraphicsView(parent)
    , m_scene(nullptr)
    , m_zoomLevel(1.0)
    , m_minZoom(0.1)
    , m_maxZoom(5.0)
    , m_zoomStep(1.2)
    , m_showGrid(true)
    , m_snapToGrid(true)
    , m_interactionMode(SelectMode)
    , m_isPanning(false)
{
    setupView();
}

//-----------------------------------------------------------

DiagramView::DiagramView(
    DiagramScene* scene, 
    QWidget* parent
)
    : QGraphicsView(scene, parent)
    , m_scene(scene)
    , m_zoomLevel(1.0)
    , m_minZoom(0.1)
    , m_maxZoom(5.0)
    , m_zoomStep(1.2)
    , m_showGrid(true)
    , m_snapToGrid(true)
    , m_interactionMode(SelectMode)
    , m_isPanning(false)
{
    setupView();

    if (m_scene) {
        connect(m_scene, &DiagramScene::changed, this, &DiagramView::onSceneChanged);
    }
}

//-----------------------------------------------------------

void DiagramView::setDiagramScene(
    DiagramScene* scene
)
{
    if (m_scene) {
        disconnect(m_scene, &DiagramScene::changed, this, &DiagramView::onSceneChanged);
    }

    m_scene = scene;
    setScene(scene);

    if (m_scene) {
        connect(m_scene, &DiagramScene::changed, this, &DiagramView::onSceneChanged);
    }
}

//-----------------------------------------------------------

void DiagramView::zoomIn()
{
    setZoomLevel(m_zoomLevel * m_zoomStep);
}

//-----------------------------------------------------------

void DiagramView::zoomOut()
{
    setZoomLevel(m_zoomLevel / m_zoomStep);
}

//-----------------------------------------------------------

void DiagramView::zoomToFit()
{
    if (!m_scene) {
        return;
    }

    QRectF itemsRect = m_scene->itemsBoundingRect();
    if (itemsRect.isEmpty()) {
        return;
    }

    itemsRect.adjust(-50, -50, 50, 50);

    fitInView(itemsRect, Qt::KeepAspectRatio);

    QTransform transform = this->transform();
    m_zoomLevel = transform.m11(); 

    constrainZoom();
    emit zoomChanged(m_zoomLevel);
}

//-----------------------------------------------------------

void DiagramView::zoomToActualSize()
{
    setZoomLevel(1.0);
}

//-----------------------------------------------------------

void DiagramView::setZoomLevel(
    qreal level
)
{
    qreal newZoom = qBound(m_minZoom, level, m_maxZoom);

    if (qAbs(newZoom - m_zoomLevel) < 0.01) {
        return;
    }

    qreal scaleFactor = newZoom / m_zoomLevel;
    scale(scaleFactor, scaleFactor);

    m_zoomLevel = newZoom;
    emit zoomChanged(m_zoomLevel);
}

//-----------------------------------------------------------

void DiagramView::centerOnDiagram()
{
    if (!m_scene) {
        return;
    }

    QRectF itemsRect = m_scene->itemsBoundingRect();
    if (!itemsRect.isEmpty()) {
        centerOn(itemsRect.center());
    }
    else {
        centerOn(0, 0);
    }
}

//-----------------------------------------------------------

void DiagramView::panTo(
    const QPointF& point
) // Changes the focus/center to a specific point
{
    centerOn(point);
}

//-----------------------------------------------------------

void DiagramView::setShowGrid(
    bool show
)
{
}

//-----------------------------------------------------------

void DiagramView::setSnapToGrid(
    bool snap
)
{
}

//-----------------------------------------------------------

void DiagramView::setInteractionMode(
    InteractionMode mode
)
{
    if (m_interactionMode != mode) {
        m_interactionMode = mode;

        switch (mode) {
            case SelectMode:
                setCursor(Qt::ArrowCursor);
                setDragMode(QGraphicsView::RubberBandDrag);
                break;
            case CreateEntityMode:
            case CreateRelationshipMode:
            case CreateAttributeMode:
                setCursor(Qt::CrossCursor);
                setDragMode(QGraphicsView::NoDrag);
                break;
            case ConnectMode:
                setCursor(Qt::PointingHandCursor);
                setDragMode(QGraphicsView::NoDrag);
                break;
        }

        emit interactionModeChanged(mode);
    }
}

//-----------------------------------------------------------

void DiagramView::wheelEvent(
    QWheelEvent* event
)
{
    if (event->modifiers() & Qt::ControlModifier) {
        const qreal scaleFactor = event->angleDelta().y() > 0 ? m_zoomStep : 1.0 / m_zoomStep;
        setZoomLevel(m_zoomLevel * scaleFactor);
        event->accept();
    }
    else {
        QGraphicsView::wheelEvent(event);
    }
}

//-----------------------------------------------------------

void DiagramView::mousePressEvent(
    QMouseEvent* event
)
{
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)) {
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && m_interactionMode != SelectMode) {
        // Modo de criação
        QPointF scenePos = mapToScene(event->pos());
        ElementType elementType = getModeElementType();

        if (elementType != ElementType::Unknown) {
            emit elementCreationRequested(elementType, scenePos);
        }

        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

//-----------------------------------------------------------

void DiagramView::mouseMoveEvent(
    QMouseEvent* event
)
{
    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();

        QPointF sceneDelta = mapToScene(delta) - mapToScene(QPoint(0, 0));

        QScrollBar* hBar = horizontalScrollBar();
        QScrollBar* vBar = verticalScrollBar();

        hBar->setValue(hBar->value() - sceneDelta.x());
        vBar->setValue(vBar->value() - sceneDelta.y());

        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

//-----------------------------------------------------------

void DiagramView::mouseReleaseEvent(
    QMouseEvent* event
)
{
    if (m_isPanning && (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier))) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------

void DiagramView::keyPressEvent(
    QKeyEvent* event
)
{
    switch (event->key()) {
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        if (event->modifiers() & Qt::ControlModifier) {
            zoomIn();
            event->accept();
            return;
        }
        break;

    case Qt::Key_Minus:
        if (event->modifiers() & Qt::ControlModifier) {
            zoomOut();
            event->accept();
            return;
        }
        break;

    case Qt::Key_0:
        if (event->modifiers() & Qt::ControlModifier) {
            zoomToActualSize();
            event->accept();
            return;
        }
        break;

    case Qt::Key_F:
        if (event->modifiers() & Qt::ControlModifier) {
            zoomToFit();
            event->accept();
            return;
        }
        break;

    case Qt::Key_Escape:
        setInteractionMode(SelectMode);
        event->accept();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}

//-----------------------------------------------------------

void DiagramView::resizeEvent(
    QResizeEvent* event
)
{
    QGraphicsView::resizeEvent(event);
    updateZoom();
}

//-----------------------------------------------------------

void DiagramView::drawBackground(
    QPainter* painter, 
    const QRectF& rect
)
{
    QGraphicsView::drawBackground(painter, rect);
}

//-----------------------------------------------------------

void DiagramView::onSceneChanged()
{
    update();
}

//-----------------------------------------------------------

void DiagramView::setupView()
{
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    setDragMode(QGraphicsView::RubberBandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setInteractive(true);

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setBackgroundBrush(QBrush(Qt::white));
}

//-----------------------------------------------------------

void DiagramView::updateZoom()
{
    constrainZoom();
}

//-----------------------------------------------------------

void DiagramView::constrainZoom()
{
    if (m_zoomLevel < m_minZoom) {
        setZoomLevel(m_minZoom);
    }
    else if (m_zoomLevel > m_maxZoom) {
        setZoomLevel(m_maxZoom);
    }
}

//-----------------------------------------------------------

QPointF DiagramView::mapToScene(
    const QPoint& point
) const
{
    return QGraphicsView::mapToScene(point);
}

//-----------------------------------------------------------

ElementType DiagramView::getModeElementType() const
{
    switch (m_interactionMode) {
    case CreateEntityMode:
        return ElementType::Entity;
    case CreateRelationshipMode:
        return ElementType::Relationship;
    case CreateAttributeMode:
        return ElementType::Attribute;
    default:
        return ElementType::Unknown;
    }
}

//-----------------------------------------------------------