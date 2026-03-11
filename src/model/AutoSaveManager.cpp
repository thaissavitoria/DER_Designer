#include "AutoSaveManager.h"
#include "model/DiagramSerializer.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>

// -----------------------------------------------------------------------------------------------------

AutoSaveManager::AutoSaveManager(
  QObject* parent
)
  : QObject(parent)
  , m_timer(new QTimer(this))
  , m_enabled(true)
  , m_interval(150000)
{
  const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  m_backupDirectory = appDataPath + "/autosave_DERDesigner";

  const QDir backupDir(m_backupDirectory);
  if (!backupDir.exists()) {
    backupDir.mkpath(".");
  }

  connect(
    m_timer,
    &QTimer::timeout,
    this,
    &AutoSaveManager::onTimerTriggered
  );
}

// -----------------------------------------------------------------------------------------------------

void AutoSaveManager::setCurrentFileName(
  const QString& fileName
)
{
  m_currentFileName = fileName;
}

// -----------------------------------------------------------------------------------------------------

void AutoSaveManager::setEnabled(
  bool enabled
)
{
  m_enabled = enabled;
  if (enabled) {
    start();
  }
  else {
    stop();
  }
}

// -----------------------------------------------------------------------------------------------------

void AutoSaveManager::start()
{
  if (m_enabled && !m_timer->isActive()) {
    m_timer->start(m_interval);
    emit statusMessageRequested("Salvamento automatico ativado");
  }
}

// -----------------------------------------------------------------------------------------------------

void AutoSaveManager::stop()
{
  if (m_timer->isActive()) {
    m_timer->stop();
    emit statusMessageRequested("Salvamento automatico desativado");
  }
}

// -----------------------------------------------------------------------------------------------------

void AutoSaveManager::onTimerTriggered()
{
  if (m_enabled) {
    emit saveRequested();
  }
}

// -----------------------------------------------------------------------------------------------------

void AutoSaveManager::executeSave(
  const QList<BasicElement*>& elements,
  const QList<ConnectionLine*>& connections
)
{
  if (elements.isEmpty()) {
    return;
  }

  if (!m_currentFileName.isEmpty()) {
    const bool success = DiagramSerializer::saveDiagramToFile(
      m_currentFileName,
      elements,
      connections
    );

    if (success) {
      emit mainFileSaved();
    }
  }

  DiagramSerializer::saveDiagramToFile(
    generateBackupFileName(),
    elements,
    connections
  );
}

// -----------------------------------------------------------------------------------------------------

QString AutoSaveManager::generateBackupFileName() const
{
  const QString baseFileName = m_currentFileName.isEmpty()
    ? "untitled"
    : QFileInfo(m_currentFileName).baseName();

  const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

  return QString("%1/%2_autosave_%3.json")
    .arg(m_backupDirectory)
    .arg(baseFileName)
    .arg(timestamp);
}

// -----------------------------------------------------------------------------------------------------
