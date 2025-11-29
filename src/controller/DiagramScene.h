#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QtCore/QHash>
#include <QtWidgets/QGraphicsScene>

#include "model/BasicElement.h"
#include "model/ConnectionLine.h"
#include "viewer/ElementGraphicsItem.h"

//------------------------------------------------------------------------------

class Attribute;
class ConnectionGraphicsItem;
class Entity;
class Relationship;
class RelationshipConnectionLine;

//------------------------------------------------------------------------------

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

  void addRelationshipConnection(
    RelationshipConnectionLine* connection
  );

  void removeConnection(
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
  QSet<ConnectionLine*> getSelectedConnections() const;

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

  void connectionStarted(
    ConnectionPoint* startPoint
  );

  void connectionCancelled();

private slots:
  void onSubAttributeOrAttributeOfEntityRemoved(
    Attribute* subAttribute
  );

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

  void handleRelationshipDisconnection(
    ConnectionLine* connection
  );

  bool isRelationshipToEntityConnection(
    BasicElement* startElement,
    BasicElement* endElement,
    Relationship** outRelationship,
    Entity** outEntity
  ) const;

  void removeElementConnections(
    BasicElement* element
  );

  void removeAttributeFromParents(
    Attribute* attribute
  );

  void handleAttributeConnection(
    ConnectionLine* connection
  );

};

#endif // DIAGRAMSCENE_H