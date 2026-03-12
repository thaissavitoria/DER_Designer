#ifndef AUTOSAVEMANAGER_H
#define AUTOSAVEMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include "model/ConnectionLine.h"
#include "model/BasicElement.h"

// -----------------------------------------------------------------------------------------------------

/**
 * @brief Manages automatic saving and backup versioning of diagrams
 *
 * Encapsulates all auto-save logic, including:
 * - Periodic saving to the current file
 * - Versioned backup files with timestamps
 * - Enable/disable control
 */
class AutoSaveManager : public QObject
{
  Q_OBJECT

public:
  explicit AutoSaveManager(
    QObject* parent = nullptr
  );

  void setCurrentFileName(
    const QString& fileName
  );

  void setEnabled(
    bool enabled
  );

  bool isEnabled() const { return m_enabled; }

  void start();
  void stop();

  void executeSave(
    const QList<BasicElement*>& elements,
    const QList<ConnectionLine*>& connections
  );

signals:
  void saveRequested();

  void mainFileSaved();

  void statusMessageRequested(
    const QString& message
  );

private slots:
  void onTimerTriggered();

private:
  QString generateBackupFileName() const;

  QTimer* m_timer;
  bool m_enabled;
  int m_interval;

  QString m_currentFileName;
  QString m_backupDirectory;
};

#endif // AUTOSAVEMANAGER_H

