#include "QuickAcessToolbar.h"
#include "MainWindow.h"

#include <QStyle>
#include <QAction>
#include <QFile>

//----------------------------------------------------------------------------------------------

QuickAccessToolbar::QuickAccessToolbar(
    QWidget* parent
) : QToolBar(parent), m_mainWindow(nullptr) {
    setupToolbar();
    addDefaultActions();
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::setMainWindow(
    MainWindow* mainWindow
) {
    m_mainWindow = mainWindow;
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::setupToolbar() {
    setMovable(false);
    setFloatable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize(QSize(16, 16));
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::addDefaultActions() {
    
    QAction* newAction = addActionToToolBar(style()->standardIcon(QStyle::SP_FileIcon), "Novo", "Criar novo diagrama", "Criar um novo diagrama DER");
    connect(newAction, &QAction::triggered, this, &QuickAccessToolbar::onNewAction);

    QAction* openAction = addActionToToolBar(style()->standardIcon(QStyle::SP_DirOpenIcon), "Abrir", "Abrir diagrama existente", "Abrir um diagrama DER existente");
    connect(openAction, &QAction::triggered, this, &QuickAccessToolbar::onOpenAction);

    QAction* saveAction = addActionToToolBar(style()->standardIcon(QStyle::SP_DialogSaveButton), "Salvar", "Salvar diagrama atual", "Salvar o diagrama atual");
    connect(saveAction, &QAction::triggered, this, &QuickAccessToolbar::onSaveAction);

    addSeparator();
    
    QAction* undoAction = addActionToToolBar(QIcon(":/undo"), "Desfazer", "Desfazer última ação", "Desfazer a última ação realizada");
    connect(undoAction, &QAction::triggered, this, &QuickAccessToolbar::onUndoAction);

    QAction* redoAction = addActionToToolBar(QIcon(":/redo"), "Refazer", "Refazer última ação", "Refazer a última ação desfeita");
    connect(redoAction, &QAction::triggered, this, &QuickAccessToolbar::onRedoAction);

    addSeparator();

    QAction* exportAction = addActionToToolBar(QIcon(":/export"), "Exportar", "Exportar diagrama", "Exportar diagrama para arquivo de imagem");
    connect(exportAction, &QAction::triggered, this, &QuickAccessToolbar::onExportAction);

    QAction* importAction = addActionToToolBar(QIcon(":/import"), "Importar", "Importar arquivo", "Importar arquivo para o diagrama");
    connect(importAction, &QAction::triggered, this, &QuickAccessToolbar::onImportAction);

    addSeparator();

    QAction* helpAction = addActionToToolBar(style()->standardIcon(QStyle::SP_MessageBoxQuestion), "Ajuda", "Mostrar ajuda", "Mostrar ajuda da aplicação");
    connect(helpAction, &QAction::triggered, this, &QuickAccessToolbar::onHelpAction);

    QAction* aboutAction = addActionToToolBar(style()->standardIcon(QStyle::SP_MessageBoxInformation), "Sobre", "Sobre o DER Designer", "Mostrar informações sobre o DER Designer");
    connect(aboutAction, &QAction::triggered, this, &QuickAccessToolbar::onAboutAction);
}

//----------------------------------------------------------------------------------------------

QAction* QuickAccessToolbar::addActionToToolBar(
    const QIcon& icon,
	const QString& actionName,
	const QString& toolTip,
	const QString& statusTip
) {
    QAction* newAction = addAction(icon, actionName);
    newAction->setToolTip(toolTip);
    newAction->setStatusTip(statusTip);
	return newAction;
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onNewAction() {
    if (m_mainWindow) {
        m_mainWindow->newFile();
    }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onSaveAction() {
    if (m_mainWindow) {
        m_mainWindow->saveFile();
    }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onOpenAction() {
    if (m_mainWindow) {
        m_mainWindow->openFile();
    }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onExportAction() {
    if (m_mainWindow) {
        m_mainWindow->exportDiagram();
    }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onUndoAction() {
    // Funcionalidade ainda não implementada
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onRedoAction() {
    // Funcionalidade ainda não implementada
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onImportAction() {
    // Funcionalidade ainda não implementada
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onHelpAction() {
    if (m_mainWindow) {
        m_mainWindow->showHelp();
    }
}

//----------------------------------------------------------------------------------------------

void QuickAccessToolbar::onAboutAction() {
    if (m_mainWindow) {
        m_mainWindow->showAbout();
    }
}

//----------------------------------------------------------------------------------------------