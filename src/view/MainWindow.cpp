#include "MainWindow.h"
#include <QtCore/QFileInfo>

#include <QtGui/QAction>
#include <QtGui/QCloseEvent>
#include <QtGui/QIcon>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>

#include "QuickAcessToolbar.h"
#include "model/DiagramFileManager.h"
#include "model/ElementFactory.h"

// -----------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , m_centralWidget(nullptr)
  , m_mainLayout(nullptr)
  , m_mainSplitter(nullptr)
  , m_diagramWidget(nullptr)
  , m_diagramLayout(nullptr)
  , m_diagramScene(nullptr)
  , m_graphicsView(nullptr)
  , m_sideTabWidget(nullptr)
  , m_drawingTab(nullptr)
  , m_propertiesTab(nullptr)
  , m_drawingLayout(nullptr)
  , m_drawingLabel(nullptr)
  , m_propertiesLayout(nullptr)
  , m_propertiesPanel(nullptr)
  , m_menuBar(nullptr)
  , m_fileMenu(nullptr)
  , m_helpMenu(nullptr)
  , m_statusBar(nullptr)
  , m_statusLabel(nullptr)
  , m_isModified(false)
  , m_autoSaveManager(new AutoSaveManager(this))
  , m_fileManager(nullptr)
{
  setupUI();
  m_fileManager = new DiagramFileManager(m_diagramScene, this);
  setupAutoSave();
  connectSignals();
  updateWindowTitle();
  updateStatusBar("Pronto");

  resize(1200, 800);

  setWindowState(Qt::WindowMaximized);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::setupUI()
{
  createMenuBar();
  createStatusBar();
  createCentralWidget();

  setWindowTitle("DER Designer");
  setMinimumSize(800, 600);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createMenuBar()
{
  m_menuBar = menuBar();

  auto quickAccess = new QuickAccessToolbar(this);

  addToolBar(Qt::TopToolBarArea, quickAccess);

  createFileMenu();
  createHelpMenu();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createFileMenu()
{
  m_fileMenu = m_menuBar->addMenu("&Arquivo");

  QAction* newAction = createAction("&Novo", QKeySequence::New, "Criar um novo diagrama DER", m_fileMenu);
  connect(newAction, &QAction::triggered, this, &MainWindow::newFile);

  QAction* openAction = createAction("&Abrir...", QKeySequence::Open, "Abrir um diagrama DER existente", m_fileMenu);
  connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

  m_fileMenu->addSeparator();

  QAction* saveAction = createAction("&Salvar", QKeySequence::Save, "Salvar o diagrama atual", m_fileMenu);
  connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

  QAction* saveAsAction = createAction("Salvar &Como...", QKeySequence::SaveAs, "Salvar o diagrama com um novo nome", m_fileMenu);
  connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);

  m_fileMenu->addSeparator();

  QAction* autoSaveAction = new QAction("Salvamento &Automatico", this);
  autoSaveAction->setCheckable(true);
  autoSaveAction->setChecked(m_autoSaveManager->isEnabled());
  autoSaveAction->setStatusTip("Ativar/desativar salvamento automatico");
  connect(
    autoSaveAction,
    &QAction::toggled,
    this,
    [this](bool checked) {
      m_autoSaveManager->setEnabled(checked);
    }
  );
  m_fileMenu->addAction(autoSaveAction);

  m_fileMenu->addSeparator();

  QAction* exportAction = createAction("&Exportar...", QKeySequence::UnknownKey, "Exportar diagrama para imagem", m_fileMenu);
  connect(exportAction, &QAction::triggered, this, &MainWindow::exportDiagram);

  m_fileMenu->addSeparator();

  QAction* exitAction = createAction("Sai&r", QKeySequence::Quit, "Sair da aplicação", m_fileMenu);
  connect(exitAction, &QAction::triggered, this, &MainWindow::exitApplication);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createHelpMenu()
{
  m_helpMenu = m_menuBar->addMenu("&Ajuda");

  QAction* helpAction = createAction("&Ajuda", QKeySequence::HelpContents, "Mostrar ajuda da aplicação", m_helpMenu);
  connect(helpAction, &QAction::triggered, this, &MainWindow::showHelp);

  m_helpMenu->addSeparator();

  QAction* aboutAction = createAction("&Sobre", QKeySequence::UnknownKey, "Sobre o DER Designer", m_helpMenu);
  connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

// -----------------------------------------------------------------------------------------------------

QAction* MainWindow::createAction(
  const QString& text,
  QKeySequence shortcut,
  const QString& statusTip,
  QMenu* parentMenu
)
{
  auto action = new QAction(text, this);
  action->setShortcut(shortcut);
  action->setStatusTip(statusTip);
  parentMenu->addAction(action);
  return action;
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createStatusBar()
{
  m_statusBar = statusBar();
  m_statusLabel = new QLabel();
  m_statusBar->addWidget(m_statusLabel);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createCentralWidget()
{
  m_centralWidget = new QWidget();
  setCentralWidget(m_centralWidget);

  m_mainLayout = new QHBoxLayout(m_centralWidget);
  m_mainLayout->setContentsMargins(5, 5, 5, 5);
  m_mainLayout->setSpacing(5);

  m_mainSplitter = new QSplitter(Qt::Horizontal);
  m_mainLayout->addWidget(m_mainSplitter);

  createDiagramArea();
  createSidePanel();

  m_mainSplitter->setStretchFactor(0, 3); // Diagram area
  m_mainSplitter->setStretchFactor(1, 1); // Side panel

  QList<int> initialSizes;
  initialSizes << 900 << 300;
  m_mainSplitter->setSizes(initialSizes);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createSidePanel()
{
  m_sideTabWidget = new QTabWidget();
  m_sideTabWidget->setTabPosition(QTabWidget::North);
  m_sideTabWidget->setMaximumWidth(350);
  m_sideTabWidget->setMinimumWidth(250);

  m_drawingTab = new QWidget();
  m_drawingLayout = new QVBoxLayout(m_drawingTab);
  m_drawingLayout->setContentsMargins(10, 10, 10, 10);
  m_drawingLayout->setSpacing(10);

  m_drawingLayout->addWidget(createDrawingButton("Entity", ElementType::Entity));
  m_drawingLayout->addWidget(createDrawingButton("Attribute", ElementType::Attribute));
  m_drawingLayout->addWidget(createDrawingButton("Relationship", ElementType::Relationship));
  m_drawingLayout->addWidget(createDrawingButton("WeakEntity", ElementType::WeakEntity));
  m_drawingLayout->addWidget(createDrawingButton("IdentifyingRelationship", ElementType::IdentifyingRelationship));

  m_drawingLayout->addStretch();

  m_sideTabWidget->addTab(m_drawingTab, "Desenho");

  m_propertiesTab = new QWidget();
  m_propertiesLayout = new QVBoxLayout(m_propertiesTab);
  m_propertiesLayout->setContentsMargins(10, 10, 10, 10);
  m_propertiesLayout->setSpacing(5);

  auto propertiesLabel = new QLabel("Propriedades:");
  propertiesLabel->setStyleSheet("font-weight: bold;");
  m_propertiesLayout->addWidget(propertiesLabel);

  m_propertiesPanel = new PropertiesPanel(m_diagramScene, m_propertiesTab);
  m_propertiesLayout->addWidget(m_propertiesPanel);

  m_sideTabWidget->addTab(m_propertiesTab, "Propriedades");

  m_mainSplitter->addWidget(m_sideTabWidget);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (m_isModified) {
    QMessageBox::StandardButton ret = QMessageBox::warning(this,
      "DER Designer",
      "O diagrama foi modificado.\n"
      "Deseja salvar as alterações?",
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
      saveFile();
      event->accept();
    }
    else if (ret == QMessageBox::Cancel) {
      event->ignore();
      return;
    }
  }
  event->accept();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::newFile()
{
  if (m_isModified) {
    QMessageBox::StandardButton ret = QMessageBox::warning(this,
      "DER Designer",
      "O diagrama atual foi modificado.\n"
      "Deseja salvar as alterações?",
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
      saveFile();
    }
    else if (ret == QMessageBox::Cancel) {
      return;
    }
  }

  m_fileManager->newDiagram();
  m_isModified = false;
  updateWindowTitle();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::openFile()
{
  if (m_isModified) {
    QMessageBox::StandardButton ret = QMessageBox::warning(
      this,
      "DER Designer",
      "O diagrama atual foi modificado.\n"
      "Deseja salvar as alterações?",
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Save) {
      saveFile();
    }
    else if (ret == QMessageBox::Cancel) {
      return;
    }
  }

  QString fileName = QFileDialog::getOpenFileName(
    this,
    "Abrir Diagrama DER",
    "",
    "Arquivos JSON (*.json);;Todos os arquivos (*)"
  );

  if (fileName.isEmpty()) {
    return;
  }

  if (!m_fileManager->openDiagram(fileName)) {
    QMessageBox::warning(
      this,
      "Erro ao Abrir",
      "Arquivo inválido ou corrompido."
    );
    return;
  }

  m_isModified = false;
  m_autoSaveManager->setCurrentFileName(fileName);
  updateWindowTitle();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::saveFile()
{
  if (m_fileManager->currentFileName().isEmpty()) {
    saveAsFile();
    return;
  }

  if (!m_diagramScene) {
    updateStatusBar("Erro: Cena do diagrama não encontrada");
    return;
  }

  if (!m_fileManager->saveDiagram(m_fileManager->currentFileName())) {
    QMessageBox::warning(
      this,
      "Erro ao Salvar",
      QString("Não foi possível salvar o arquivo:\n%1").arg(m_fileManager->currentFileName())
    );
    return;
  }

  m_isModified = false;
  updateWindowTitle();
}

//----------------------------------------------------------------------------------------------

void MainWindow::saveAsFile()
{
  const QString fileName = QFileDialog::getSaveFileName(
    this,
    "Salvar Diagrama DER",
    "",
    "Arquivos JSON (*.json);;Todos os arquivos (*)"
  );

  if (fileName.isEmpty()) {
    return;
  }

  if (!m_fileManager->saveDiagram(fileName)) {
    QMessageBox::warning(
      this,
      "Erro ao Salvar",
      QString("Não foi possível salvar o arquivo:\n%1").arg(fileName)
    );
    return;
  }

  m_isModified = false;
  m_autoSaveManager->setCurrentFileName(fileName);
  updateWindowTitle();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::exportDiagram()
{
  if (!m_diagramScene) {
    QMessageBox::warning(
      this,
      "Erro ao Exportar",
      "Nenhum diagrama disponível para exportar."
    );
    updateStatusBar("Erro: Nenhum diagrama para exportar");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(
    this,
    "Exportar Diagrama",
    "",
    "PNG (*.png);;JPEG (*.jpeg);;JPG (*.jpg)"
  );

  if (fileName.isEmpty()) {
    return;
  }

  const QFileInfo fileInfo(fileName);
  const QString format = fileInfo.suffix().toUpper();

  if (m_fileManager->exportDiagram(fileName, format)) {
    QMessageBox::information(
      this,
      "Exportação Concluída",
      QString("Diagrama exportado com sucesso:\n%1").arg(fileName)
    );
  }
  else {
    QMessageBox::warning(
      this,
      "Erro ao Exportar",
      QString("Não foi possível exportar o diagrama:\n%1").arg(fileName)
    );
  }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::exitApplication()
{
  close();
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::createDiagramArea()
{
  m_diagramWidget = new QWidget();
  m_diagramLayout = new QVBoxLayout(m_diagramWidget);
  m_diagramLayout->setContentsMargins(0, 0, 0, 0);
  m_diagramLayout->setSpacing(0);

  m_diagramScene = new DiagramScene(this);

  m_graphicsView = new DiagramView(m_diagramScene);
  m_graphicsView->setBackgroundBrush(QColor(245, 245, 245));

  connect(m_graphicsView, &DiagramView::elementDropped, this, &MainWindow::onElementDropped);

  m_diagramLayout->addWidget(m_graphicsView);
  m_mainSplitter->addWidget(m_diagramWidget);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::onElementDropped(
  const QString& elementType,
  const QPointF& position
)
{
  if (auto element = ElementFactory::createElement(elementType, m_diagramScene)) {
    element->setPosition(position);
    m_diagramScene->addElement(element);

    m_isModified = true;
    updateWindowTitle();
    updateStatusBar(QString("%1 adicionado na posição (%2, %3)")
      .arg(element->stringElementType())
      .arg(position.x(), 0, 'f', 1)
      .arg(position.y(), 0, 'f', 1));
  }
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::connectSignals()
{
  if (m_diagramScene) {
    connect(m_diagramScene, &DiagramScene::selectionChanged, this, &MainWindow::onSelectionChanged);
  }

  if (m_propertiesPanel) {
    connect(m_propertiesPanel, &PropertiesPanel::diagramModified, this, [this]() {
      m_isModified = true;
      updateWindowTitle();
    });
    connect(m_propertiesPanel, &PropertiesPanel::statusMessageRequested, this, &MainWindow::updateStatusBar);
  }

  connect(m_fileManager, &DiagramFileManager::statusMessageRequested, this, &MainWindow::updateStatusBar);
  connect(m_autoSaveManager, &AutoSaveManager::statusMessageRequested, this, &MainWindow::updateStatusBar);
  connect(m_autoSaveManager, &AutoSaveManager::mainFileSaved, this, [this]() {
    m_isModified = false;
    updateWindowTitle();
  });
  connect(m_autoSaveManager, &AutoSaveManager::saveRequested, this, &MainWindow::onAutoSaveTriggered);
}

//----------------------------------------------------------------------------------------------

void MainWindow::onSelectionChanged()
{
  if (!m_propertiesPanel || !m_diagramScene) {
    return;
  }

  m_propertiesPanel->refresh(
    m_diagramScene->getSelectedElements(),
    m_diagramScene->getSelectedConnections()
  );
}

//----------------------------------------------------------------------------------------------

DraggableButton* MainWindow::createDrawingButton(
  const QString& elementTypeName,
  ElementType elementType
)
{
  auto button = new DraggableButton(elementTypeName);
  connect(button, &QPushButton::clicked, this, [this, elementType]() {
    onAddElementClicked(elementType);
  });
  return button;
}

//----------------------------------------------------------------------------------------------

void MainWindow::onAddElementClicked(
  ElementType elementType
)
{
  if (!m_diagramScene) {
    return;
  }

  auto element = ElementFactory::createElement(elementType, m_diagramScene);
  m_diagramScene->addElement(element);
  m_isModified = true;
  updateWindowTitle();
  updateStatusBar(element->stringElementType() + " add");
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::showAbout()
{
  QMessageBox::about(this, "Sobre DER Designer",
    "<h3>DER Designer</h3>"
    "<p>Thaissa Vitoria G.D. de Sousa, Prof. Dr. Andre L. Maravilha Silva, Prof. Me. Eduardo Gabriel Reis Miranda</p>"
    "<p>Ferramenta para criação de Diagramas Entidade-Relacionamento "
    "baseados na notação de Peter Chen.</p>"
    "<p>Desenvolvido com Qt e C++.</p>"
    "<p>Versão 1.0</p>");
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::showHelp()
{
  QMessageBox::information(this, "Ajuda",
    "<h3>Como usar o DER Designer</h3>"
    "<p><b>Aba Desenho:</b> Selecione os elementos para adicionar ao diagrama.</p>"
    "<p><b>Aba Propriedades:</b> Visualize e edite propriedades do elemento selecionado.</p>"
    "<p><b>Área de Desenho:</b> Clique e arraste para criar e posicionar elementos.</p>"
    "<p><b>Menu Arquivo:</b> Criar, abrir, salvar e exportar diagramas.</p>");
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::updateWindowTitle()
{
  QString title = "DER Designer";
  if (m_fileManager && !m_fileManager->currentFileName().isEmpty()) {
    title += " - " + QFileInfo(m_fileManager->currentFileName()).baseName();
  }
  if (m_isModified) {
    title += " *";
  }
  setWindowTitle(title);
}

//----------------------------------------------------------------------------------------------

void MainWindow::updateStatusBar(const QString& message)
{
  m_statusLabel->setText(message);
}

// -----------------------------------------------------------------------------------------------------

void MainWindow::setupAutoSave()
{
  if (m_autoSaveManager->isEnabled()) {
    m_autoSaveManager->start();
  }
}

//----------------------------------------------------------------------------------------------

void MainWindow::onAutoSaveTriggered()
{
  if (!m_diagramScene || !m_isModified) {
    return;
  }

  m_autoSaveManager->setCurrentFileName(m_fileManager->currentFileName());
  m_autoSaveManager->executeSave(
    m_diagramScene->getAllElements(),
    m_diagramScene->getAllConnections()
  );
}

//----------------------------------------------------------------------------------------------
