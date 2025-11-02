#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtCore/QObject>
#include <QtCore/QPointF>

#include "model/BasicElement.h"

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QResizeEvent;
QT_END_NAMESPACE

class DiagramScene;

/**
 * @brief View para exibir e interagir com o diagrama ERD
 *
 * Esta classe herda de QGraphicsView e fornece:
 * - Visualização da cena do diagrama
 * - Controles de zoom e pan
 * - Gerenciamento de eventos de entrada
 * - Interface para criação de elementos
 */
class DiagramView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit DiagramView(
        QWidget* parent = nullptr
    );

    explicit DiagramView(
        DiagramScene* scene, 
        QWidget* parent = nullptr
    );
    virtual ~DiagramView() = default;

    void setDiagramScene(
        DiagramScene* scene
    );

    DiagramScene* diagramScene() const { return m_scene;};

    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void zoomToActualSize();
    void setZoomLevel(qreal level);
    qreal zoomLevel() const { return m_zoomLevel; }

    // Navegação
    void centerOnDiagram();
    void panTo(const QPointF& point);

    // Configurações de visualização
    void setShowGrid(bool show);
    bool isShowGrid() const { return m_showGrid; }

    void setSnapToGrid(bool snap);
    bool isSnapToGrid() const { return m_snapToGrid; }

    // Modo de interação
    enum InteractionMode {
        SelectMode,
        CreateEntityMode,
        CreateRelationshipMode,
        CreateAttributeMode,
        ConnectMode
    };

    void setInteractionMode(InteractionMode mode);
    InteractionMode interactionMode() const { return m_interactionMode; }

signals:
    void zoomChanged(qreal level);
    void interactionModeChanged(InteractionMode mode);
    void elementCreationRequested(ElementType type, const QPointF& position);

protected:
    // Eventos
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // Desenho
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private slots:
    void onSceneChanged();

private:
    void setupView();
    void updateZoom();
    void constrainZoom();

    // Utilitários
    QPointF mapToScene(const QPoint& point) const;
    ElementType getModeElementType() const;

    // Dados
    DiagramScene* m_scene;

    // Estado de zoom
    qreal m_zoomLevel;
    qreal m_minZoom;
    qreal m_maxZoom;
    qreal m_zoomStep;

    // Configurações
    bool m_showGrid;
    bool m_snapToGrid;

    // Modo de interação
    InteractionMode m_interactionMode;

    // Estado de pan
    bool m_isPanning;
    QPoint m_lastPanPoint;
};

#endif // DIAGRAMVIEW_H