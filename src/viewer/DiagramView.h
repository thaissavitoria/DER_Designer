#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QKeyEvent>

class DiagramScene;

class DiagramView : public QGraphicsView
{
  Q_OBJECT

public:
  explicit DiagramView(
    DiagramScene* scene,
    QWidget* parent = nullptr
  );
  virtual ~DiagramView() = default;

  void zoomIn();
  void zoomOut();
  void resetZoom();
  void fitToView();
  void setZoomLevel(
    qreal zoomLevel
  );
  qreal getZoomLevel() const;

protected:
  void dragEnterEvent(
    QDragEnterEvent* event
  ) override;

  void dragMoveEvent(
    QDragMoveEvent* event
  ) override;

  void dropEvent(
    QDropEvent* event
  ) override;

  void wheelEvent(
    QWheelEvent* event
  ) override;

  void keyPressEvent(
    QKeyEvent* event
  ) override;

  void drawBackground(
    QPainter* painter,
    const QRectF& rect
  ) override;

signals:
  void elementDropped(
    const QString& elementType,
    const QPointF& position
  );

private:
  bool isValidElementType(
    const QString& elementType
  ) const;

  void applyZoom(
    qreal scaleFactor,
    const QPointF& centerPoint = QPointF()
  );

  void drawGrid(
    QPainter* painter,
    const QRectF& rect
  );

  const qreal MIN_ZOOM = 0.1;
  const qreal MAX_ZOOM = 5.0;
  const qreal ZOOM_STEP = 1.25;
  const qreal DEFAULT_ZOOM = 1.0;
  const qreal GRID_SIZE = 30.0;

  qreal m_currentZoom;

  bool m_gridVisible = true; /*implementar grid on off depois?*/
};

#endif // DIAGRAMVIEW_H