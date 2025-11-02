#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QtCore/QHash>
#include <QtWidgets/QGraphicsScene>

#include "model/BasicElement.h"
#include "viewer/ElementGraphicsItem.h"

/**
 * @brief Custom scene to manage ERD elements
 *
 * Manages the interaction BasicElement and ElementGraphicsItem
 */
class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit DiagramScene(
        QObject* parent = nullptr
    );

    virtual ~DiagramScene();

    void addElement(
        BasicElement* element
    );

    void removeElement(
        BasicElement* element
    );

    void removeElement(
        const QString& elementId
    );

    BasicElement* findElement(
        const QString& id
    ) const;

    ElementGraphicsItem* findGraphicsItem(
        const QString& elementId
    ) const;

    ElementGraphicsItem* findGraphicsItem(
        BasicElement* element
    ) const;

    QList<BasicElement*> getAllElements() const;
    QList<BasicElement*> getSelectedElements() const;

    void selectElement(
        BasicElement* element,
        bool selected = true
    );

    void selectElement(
        const QString& elementId,
        bool selected = true
    );

    void clearSelection();
    void selectAll();

    void deleteSelected();
    void duplicateSelected();

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

    void keyPressEvent(
        QKeyEvent* event
    ) override;

signals:
    void elementAdded(
        BasicElement* element
    );

    void elementRemoved(
        BasicElement* element
    );

    void elementSelected(
        BasicElement* element
    );

    void elementDeselected(
        BasicElement* element
    );

    void selectionChanged();

private:
    QHash<BasicElement*, ElementGraphicsItem*> m_elementToItem;
    QHash<QString, BasicElement*> m_elements;

    bool m_selectionRectActive;
    QPointF m_selectionStartPoint;
    QGraphicsRectItem* m_selectionRect;

    void updateSelectionRect(
        const QPointF& currentPoint
    );
    void finishSelectionRect();
    void createSelectionRect();
    void destroySelectionRect();
};

#endif // DIAGRAMSCENE_H