#ifndef DIAGRAMFILEMANAGER_H
#define DIAGRAMFILEMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QString>

class DiagramScene;

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Constructs the file manager associated with a scene.
 * @param scene The diagram scene to be managed.
 * @param parent Optional Qt parent object.
 */

class DiagramFileManager : public QObject
{
  Q_OBJECT

public:
  explicit DiagramFileManager(
    DiagramScene* scene,
    QObject* parent = nullptr
  );

  ~DiagramFileManager() = default;

  bool newDiagram();

  bool openDiagram(
    const QString& fileName
  );

  bool saveDiagram(
    const QString& fileName
  );

  bool exportDiagram(
    const QString& fileName,
    const QString& format
  );

  QString currentFileName() const;

signals:
  void statusMessageRequested(
    const QString& message
  );

  void diagramLoaded(
    const QString& fileName
  );

  void diagramSaved(
    const QString& fileName
  );

private:
  DiagramScene* m_scene;
  QString m_currentFileName;
};

#endif // DIAGRAMFILEMANAGER_H
