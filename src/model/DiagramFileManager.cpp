#include "model/DiagramFileManager.h"
#include "model/BasicElement.h"
#include "model/ConnectionLine.h"
#include "model/DiagramSerializer.h"
#include "view/DiagramScene.h"

#include <QtCore/QFileInfo>
#include <QtCore/QJsonObject>

// -----------------------------------------------------------------------------------------------------

DiagramFileManager::DiagramFileManager(
  DiagramScene* scene,
  QObject* parent
)
  : QObject(parent)
  , m_scene(scene)
{
}

//----------------------------------------------------------------------------------------------

bool DiagramFileManager::newDiagram()
{
  m_scene->clearDiagram();
  m_currentFileName.clear();
  emit statusMessageRequested("Novo diagrama criado");
  return true;
}

//----------------------------------------------------------------------------------------------

bool DiagramFileManager::openDiagram(
  const QString& fileName
)
{
  QJsonObject diagramData;

  if (!DiagramSerializer::loadDiagramFromFile(fileName, diagramData)) {
    emit statusMessageRequested("Erro ao abrir arquivo");
    return false;
  }

  m_scene->clearDiagram();

  if (!DiagramSerializer::loadDiagram(diagramData, m_scene)) {
    emit statusMessageRequested("Erro ao carregar elementos");
    return false;
  }

  m_currentFileName = fileName;
  emit diagramLoaded(fileName);
  emit statusMessageRequested("Diagrama carregado: " + QFileInfo(fileName).baseName());
  return true;
}

//----------------------------------------------------------------------------------------------

bool DiagramFileManager::saveDiagram(
  const QString& fileName
)
{
  const QList<BasicElement*> elements = m_scene->getAllElements();
  const QList<ConnectionLine*> connections = m_scene->getAllConnections();

  if (!DiagramSerializer::saveDiagramToFile(fileName, elements, connections)) {
    emit statusMessageRequested("Erro ao salvar arquivo");
    return false;
  }

  m_currentFileName = fileName;
  emit diagramSaved(fileName);
  emit statusMessageRequested("Diagrama salvo: " + QFileInfo(fileName).baseName());
  return true;
}

//----------------------------------------------------------------------------------------------

bool DiagramFileManager::exportDiagram(
  const QString& fileName,
  const QString& format
)
{
  const bool success = m_scene->exportToImage(fileName, format);

  if (success) {
    emit statusMessageRequested("Diagrama exportado: " + QFileInfo(fileName).baseName());
  }
  else {
    emit statusMessageRequested("Erro ao exportar diagrama");
  }

  return success;
}

//----------------------------------------------------------------------------------------------

QString DiagramFileManager::currentFileName() const
{
  return m_currentFileName;
}

//----------------------------------------------------------------------------------------------
