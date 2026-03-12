#ifndef DIAGRAMSERIALIZER_H
#define DIAGRAMSERIALIZER_H

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

// -----------------------------------------------------------------------------------------------------

class BasicElement;
class ConnectionLine;
class DiagramScene;

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Handles JSON serialization and deserialization of diagram elements.
 *
 * Provides static utility methods to convert elements and connections
 * to/from JSON, as well as saving and loading diagrams from files.
 */

class DiagramSerializer
{
public:
  static QJsonObject elementToJsonObject(
    const BasicElement* element
  );

  static QJsonObject connectionToJsonObject(
    const ConnectionLine* connection
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

  static bool loadDiagram(
    const QJsonObject& diagramData,
    DiagramScene* scene
  );

private:
  DiagramSerializer() = default;

  static bool loadElements(
    const QJsonArray& elementsArray,
    DiagramScene* scene,
    QHash<QString, BasicElement*>& loadedElements
  );

  static bool loadConnections(
    const QJsonArray& connectionsArray,
    const QHash<QString, BasicElement*>& loadedElements,
    DiagramScene* scene
  );
};

#endif // DIAGRAMSERIALIZER_H
