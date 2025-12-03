#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

//----------------------------------------------------------------------------------------------

class BasicElement;
class ConnectionLine;

//----------------------------------------------------------------------------------------------

class JsonHelper
{
public:
  static QString elementToJson(
    const BasicElement* element
  );
    
  static QJsonObject elementToJsonObject(
    const BasicElement* element
  );

  static QJsonObject connectionToJsonObject(
    const ConnectionLine* connection
  );
    
  static bool elementFromJson(
    BasicElement* element,
    const QString& jsonString
  );
    
  static bool elementFromJsonObject(
    BasicElement* element,
    const QJsonObject& jsonObject
  );

  static QString diagramToJson(
    const QList<BasicElement*>& elements,
    const QList<ConnectionLine*>& connections
  );

  static QJsonObject diagramToJsonObject(
    const QList<BasicElement*>& elements,
    const QList<ConnectionLine*>& connections
  );

  static bool saveDiagramToFile(
    const QString& filePath,
    const QList<BasicElement*>& elements,
    const QList<ConnectionLine*>& connections
  );

  static bool loadDiagramFromFile(
    const QString& filePath,
    QJsonObject& diagramData
  );

private:
  JsonHelper() = default;
};

#endif // JSONHELPER_H