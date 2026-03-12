#include "model/DiagramSerializer.h"

#include "model/BasicElement.h"
#include "model/ConnectionLine.h"
#include "model/ConnectionPoint.h"
#include "model/ElementFactory.h"
#include "model/Relationship.h"
#include "model/RelationshipConnectionLine.h"
#include "model/RelationshipEnd.h"
#include "view/DiagramScene.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>

//----------------------------------------------------------------------------------------------

QJsonObject DiagramSerializer::elementToJsonObject(
  const BasicElement* element
)
{
  if (!element) {
    return QJsonObject();
  }

  QVariantMap data = element->serialize();
  return QJsonObject::fromVariantMap(data);
}

//----------------------------------------------------------------------------------------------

QJsonObject DiagramSerializer::connectionToJsonObject(
  const ConnectionLine* connection
)
{
  if (!connection) {
    return QJsonObject();
  }

  QVariantMap data = connection->serialize();
  return QJsonObject::fromVariantMap(data);
}

//----------------------------------------------------------------------------------------------

QJsonObject DiagramSerializer::diagramToJsonObject(
  const QList<BasicElement*>& elements,
  const QList<ConnectionLine*>& connections
)
{
  QJsonObject diagramJson;
  diagramJson["version"] = "1.0";
  diagramJson["type"] = "ERDiagram";

  QSet<QString> serializedIds;

  QJsonArray elementsArray;
  for (BasicElement* element : elements) {
    if (element && element->isValid()) {
      QJsonObject elementJson = elementToJsonObject(element);
      elementsArray.append(elementJson);
      serializedIds.insert(element->id());
    }
  }
  diagramJson["elements"] = elementsArray;

  QJsonArray connectionsArray;
  for (ConnectionLine* connection : connections) {
    if (connection && connection->isValid()) {
      QJsonObject connectionJson = connectionToJsonObject(connection);
      connectionsArray.append(connectionJson);
    }
  }
  diagramJson["connections"] = connectionsArray;

  return diagramJson;
}

//----------------------------------------------------------------------------------------------

bool DiagramSerializer::saveDiagramToFile(
  const QString& filePath,
  const QList<BasicElement*>& elements,
  const QList<ConnectionLine*>& connections
)
{
  if (filePath.isEmpty()) {
    return false;
  }

  QJsonObject diagramJson = diagramToJsonObject(elements, connections);
  QJsonDocument jsonDoc(diagramJson);

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) {
    return false;
  }

  qint64 bytesWritten = file.write(jsonDoc.toJson(QJsonDocument::Indented));
  file.close();

  return bytesWritten > 0;
}

//----------------------------------------------------------------------------------------------

bool DiagramSerializer::loadDiagramFromFile(
  const QString& filePath,
  QJsonObject& diagramData
)
{
  if (filePath.isEmpty()) {
    return false;
  }

  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QByteArray fileData = file.readAll();
  file.close();

  QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
  if (jsonDoc.isNull() || !jsonDoc.isObject()) {
    return false;
  }

  diagramData = jsonDoc.object();

  if (!diagramData.contains("elements") || !diagramData.contains("connections")) {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------

bool DiagramSerializer::loadDiagram(
  const QJsonObject& diagramData,
  DiagramScene* scene
)
{
  if (!scene) {
    return false;
  }

  QHash<QString, BasicElement*> loadedElements;

  if (!loadElements(diagramData["elements"].toArray(), scene, loadedElements)) {
    return false;
  }

  return loadConnections(diagramData["connections"].toArray(), loadedElements, scene);
}

//----------------------------------------------------------------------------------------------

bool DiagramSerializer::loadElements(
  const QJsonArray& elementsArray,
  DiagramScene* scene,
  QHash<QString, BasicElement*>& loadedElements
)
{
  for (const QJsonValue& elementValue : elementsArray) {
    QJsonObject elementJson = elementValue.toObject();
    QVariantMap elementData = elementJson.toVariantMap();

    if (!elementData.contains("type")) {
      continue;
    }

    auto elementType = static_cast<ElementType>(elementData["type"].toInt());
    auto element = ElementFactory::createElement(elementType, scene);

    if (element) {
      if (element->deserialize(elementData)) {
        scene->addElement(element);
        loadedElements[element->id()] = element;
      }
      else {
        delete element;
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------

bool DiagramSerializer::loadConnections(
  const QJsonArray& connectionsArray,
  const QHash<QString, BasicElement*>& loadedElements,
  DiagramScene* scene
)
{
  for (const QJsonValue& connectionValue : connectionsArray) {
    QJsonObject connectionJson = connectionValue.toObject();

    QString startElementId = connectionJson["startElementId"].toString();
    QString endElementId = connectionJson["endElementId"].toString();

    if (!loadedElements.contains(startElementId) || !loadedElements.contains(endElementId)) {
      continue;
    }

    BasicElement* startElement = loadedElements[startElementId];
    BasicElement* endElement = loadedElements[endElementId];

    if (!startElement || !endElement) {
      continue;
    }

    ConnectionPoint* startPoint = startElement->getConnectionPointByDirection(
      static_cast<ConnectionDirection>(connectionJson["startConnectionPointDirection"].toInt())
    );
    ConnectionPoint* endPoint = endElement->getConnectionPointByDirection(
      static_cast<ConnectionDirection>(connectionJson["endConnectionPointDirection"].toInt())
    );

    if (!startPoint || !endPoint) {
      continue;
    }

    const bool isRelationshipEntityConnection = connectionJson.contains("cardinalityOffsetX");

    if (isRelationshipEntityConnection) {
      auto relationship = qobject_cast<Relationship*>(startElement);
      BasicElement* entity = endElement;

      if (!relationship) {
        relationship = qobject_cast<Relationship*>(endElement);
        entity = startElement;
      }

      if (relationship && entity) {
        RelationshipEnd* relationshipEnd = nullptr;
        QList<RelationshipEnd*> ends = relationship->ends();

        for (RelationshipEnd* end : ends) {
          if (end->entityId() == entity->id()) {
            relationshipEnd = end;
            break;
          }
        }

        if (relationshipEnd) {
          auto relConnection = new RelationshipConnectionLine(
            startPoint,
            endPoint,
            relationshipEnd,
            scene
          );

          QVariantMap connectionData = connectionJson.toVariantMap();
          relConnection->deserialize(connectionData);
          scene->addRelationshipConnection(relConnection);
        }
      }
    }
    else {
      auto connection = new ConnectionLine(startPoint, endPoint, scene);

      QVariantMap connectionData = connectionJson.toVariantMap();
      connection->deserialize(connectionData);
      scene->addConnection(connection);
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------
