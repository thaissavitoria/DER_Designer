#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QtCore/QHash>
#include <QtWidgets/QGraphicsScene>

#include "model/BasicElement.h"
#include "model/ConnectionLine.h"
#include "viewer/ElementGraphicsItem.h"

class ConnectionGraphicsItem;

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

  void addConnection(
    ConnectionLine* connection
  );

  void removeConnection(
    ConnectionLine* connection
  );

  void removeConnectionFromContainers(
    ConnectionLine* connection
  );

  void removeConnection(
    const QString& connectionId
  );

  ConnectionLine* findConnection(
    const QString& id
  ) const;

  void startConnection(
    ConnectionPoint* startPoint
  );

  void finishConnection(
    ConnectionPoint* endPoint
  );

  void cancelConnection();

  QList<BasicElement*> getAllElements() const;
  QList<BasicElement*> getSelectedElements() const;
  QList<ConnectionLine*> getAllConnections() const;
  QList<ConnectionLine*> getSelectedConnections() const;

  void selectElement(
    BasicElement* element,
    bool selected = true
  );

  void selectElement(
    const QString& elementId,
    bool selected = true
  );

  void selectConnection(
    ConnectionLine* connection,
    bool selected = true
  );

  void selectConnection(
    const QString& connectionId,
    bool selected = true
  );

  void clearSelection();
  void selectAll();

  void deleteSelected();
  void duplicateSelected();

  bool isCreatingConnection() const { return m_isCreatingConnection; }

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

  void connectionSelected(
    ConnectionLine* connection
  );

  void connectionDeselected(
    ConnectionLine* connection
  );

  void selectionChanged();

  void connectionAdded(
    ConnectionLine* connection
  );

  void connectionStarted(
    ConnectionPoint* startPoint
  );

  void connectionFinished(
    ConnectionLine* connection
  );

  void connectionCancelled();

private:
  QHash<BasicElement*, ElementGraphicsItem*> m_elementToItem;
  QHash<QString, BasicElement*> m_elements;
  QHash<ConnectionLine*, ConnectionGraphicsItem*> m_connectionToItem;
  QHash<QString, ConnectionLine*> m_connections;

  bool m_selectionRectActive;
  QPointF m_selectionStartPoint;
  QGraphicsRectItem* m_selectionRect;

  bool m_isCreatingConnection;
  ConnectionPoint* m_connectionStartPoint;
  QGraphicsLineItem* m_temporaryConnectionLine;

  void updateSelectionRect(
    const QPointF& currentPoint
  );
  void finishSelectionRect();
  void createSelectionRect();
  void destroySelectionRect();

  void updateTemporaryConnection(
    const QPointF& endPosition
  );
};

#endif // DIAGRAMSCENE_H