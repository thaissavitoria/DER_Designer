#include "DraggableButton.h"

#include <QApplication>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>

//----------------------------------------------------------------------------------------------

DraggableButton::DraggableButton(
  const QString& elementType,
  QWidget* parent
)
  : QPushButton(parent)
  , m_elementType(elementType)
  , m_dragging(false)
{
  setAcceptDrops(false);
  setupIcon();
}

//----------------------------------------------------------------------------------------------

void DraggableButton::setElementType(
  const QString& elementType
)
{
  m_elementType = elementType;
  setupIcon();
}

//----------------------------------------------------------------------------------------------

void DraggableButton::setupIcon()
{
  QString iconPath = getIconPath();
  if (!iconPath.isEmpty()) {
    setIcon(QIcon(iconPath));
    setIconSize(QSize(140, 140));
    setText(getDisplayName());
    setToolTip(getDisplayName());

    setStyleSheet(
      "QPushButton {"
      "    border: none;"
      "    background: transparent;"
      "    padding: 0px;"
      "    text-align: left;"
      "    padding-left: 50px;"
      "    font-size: 15px;"
      "}"
      "QPushButton:hover {"
      "    background: rgba(200, 200, 200, 50);"
      "    border-radius: 4px;"
      "}"
      "QPushButton:pressed {"
      "    background: rgba(150, 150, 150, 100);"
      "    border-radius: 4px;"
      "}"
    );
  }
}

//----------------------------------------------------------------------------------------------

QString DraggableButton::getIconPath() const
{
  if (m_elementType == "Entity") {
    return ":/entidade";
  }
  else if (m_elementType == "Attribute") {
    return ":/atributo";
  }
  else if (m_elementType == "Relationship") {
    return ":/relacionamento";
  }
  else if (m_elementType == "WeakEntity") {
    return ":/entidadeFraca";
  }
  else if (m_elementType == "IdentifyingRelationship") {
    return ":/relacionamentoIdentificador";
  }

  return QString();
}

//----------------------------------------------------------------------------------------------

QString DraggableButton::getDisplayName() const
{
  if (m_elementType == "Entity") {
    return "Entidade";
  }
  else if (m_elementType == "Attribute") {
    return "Atributo";
  }
  else if (m_elementType == "Relationship") {
    return "Relacionamento";
  }
  else if (m_elementType == "WeakEntity") {
    return "Entidade Fraca";
  }
  else if (m_elementType == "IdentifyingRelationship") {
    return "Relacionamento\n Identificador";
  }

  return m_elementType.toUpper();
}

//----------------------------------------------------------------------------------------------

void DraggableButton::mousePressEvent(
  QMouseEvent* event
)
{
  if (event->button() == Qt::LeftButton) {
    m_dragStartPosition = event->pos();
    m_dragging = false;
  }

  QPushButton::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------------

void DraggableButton::mouseMoveEvent(
  QMouseEvent* event
)
{
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }

  if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
    return;
  }

  if (m_elementType.isEmpty()) {
    return;
  }

  m_dragging = true;

  auto drag = new QDrag(this);
  auto mimeData = new QMimeData();

  mimeData->setText(m_elementType);
  mimeData->setData("application/x-element-type", m_elementType.toUtf8());

  drag->setMimeData(mimeData);

  QString iconPath = getIconPath();
  QPixmap iconPixmap(iconPath);
  if (iconPixmap.isNull()) {
    iconPixmap = QPixmap(140, 140);
    iconPixmap.fill(Qt::transparent);
  }

  QPixmap dragPixmap(iconPixmap.size());
  dragPixmap.fill(Qt::transparent);

  QPainter painter(&dragPixmap);
  painter.setOpacity(0.7);
  painter.drawPixmap(0, 0, iconPixmap);
  painter.end();

  drag->setPixmap(dragPixmap);
  drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));

  Qt::DropAction dropAction = drag->exec(Qt::CopyAction);

  Q_UNUSED(dropAction)

  m_dragging = false;
}

//----------------------------------------------------------------------------------------------
