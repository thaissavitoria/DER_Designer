#include "JsonHelper.h"
#include "model/BasicElement.h"
#include "model/ConnectionLine.h"
#include "model/Attribute.h"
#include "model/Entity.h"
#include "model/Relationship.h"

#include <QtCore/QFile>
#include <QtCore/QDebug>

//----------------------------------------------------------------------------------------------

QString JsonHelper::elementToJson(
  const BasicElement* element
)
{
  if (!element) {
    return QString();
  }

  QJsonObject jsonObject = elementToJsonObject(element);
  QJsonDocument jsonDoc(jsonObject);
  return jsonDoc.toJson(QJsonDocument::Compact);
}

//----------------------------------------------------------------------------------------------

QJsonObject JsonHelper::elementToJsonObject(
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

QJsonObject JsonHelper::connectionToJsonObject(
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

bool JsonHelper::elementFromJson(
  BasicElement* element,
  const QString& jsonString
)
{
  if (!element || jsonString.isEmpty()) {
    return false;
  }

  QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
  if (jsonDoc.isNull() || !jsonDoc.isObject()) {
    return false;
  }

  return elementFromJsonObject(element, jsonDoc.object());
}

//----------------------------------------------------------------------------------------------

bool JsonHelper::elementFromJsonObject(
  BasicElement* element,
  const QJsonObject& jsonObject
)
{
  if (!element || jsonObject.isEmpty()) {
    return false;
  }

  QVariantMap data = jsonObject.toVariantMap();
  return element->deserialize(data);
}

//----------------------------------------------------------------------------------------------

QString JsonHelper::diagramToJson(
  const QList<BasicElement*>& elements,
  const QList<ConnectionLine*>& connections
)
{
  QJsonObject diagramObject = diagramToJsonObject(elements, connections);
  QJsonDocument jsonDoc(diagramObject);
  return jsonDoc.toJson(QJsonDocument::Indented);
}

//----------------------------------------------------------------------------------------------

QJsonObject JsonHelper::diagramToJsonObject(
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

bool JsonHelper::saveDiagramToFile(
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

bool JsonHelper::loadDiagramFromFile(
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