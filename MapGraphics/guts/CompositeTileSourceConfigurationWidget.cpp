#include "CompositeTileSourceConfigurationWidget.h"
#include "ui_CompositeTileSourceConfigurationWidget.h"

#include "MapTileLayerListModel.h"
#include "MapTileSourceDelegate.h"

#include <QtDebug>

CompositeTileSourceConfigurationWidget::CompositeTileSourceConfigurationWidget(QWeakPointer<CompositeTileSource> composite,
                                                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompositeTileSourceConfigurationWidget),
    _composite(composite)
{
    ui->setupUi(this);

    //Create a fancy delegate for custom drawing of our list items
    MapTileSourceDelegate * delegato = new MapTileSourceDelegate(composite,this);

    //Create the model that watches the CompositeTileSource
    MapTileLayerListModel * model = new MapTileLayerListModel(_composite,this);

    //Set the QListView to watch the model and use the delegate
    this->ui->listView->setModel(model);
    this->ui->listView->setItemDelegate(delegato);

    QItemSelectionModel * selModel = this->ui->listView->selectionModel();
    connect(selModel,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(handleCurrentSelectionChanged(QModelIndex,QModelIndex)));
}

CompositeTileSourceConfigurationWidget::~CompositeTileSourceConfigurationWidget()
{
    delete ui;
}

//private slot
void CompositeTileSourceConfigurationWidget::handleCurrentSelectionChanged(QModelIndex current, QModelIndex previous)
{
    Q_UNUSED(previous)
    bool enableGUI = current.isValid();

    this->ui->removeSourceButton->setEnabled(enableGUI);
    this->ui->opacitySlider->setEnabled(enableGUI);

    QSharedPointer<CompositeTileSource> strong = _composite.toStrongRef();
    if (strong.isNull())
        return;

    this->ui->moveDownButton->setEnabled(enableGUI &&  (strong->numSources()-1) > current.row());
    this->ui->moveUpButton->setEnabled(enableGUI && 0 < current.row());

    if (enableGUI)
    {
        qreal opacityFloat = strong->getOpacity(current.row());
        this->ui->opacitySlider->setValue(opacityFloat*100);
    }
}

//private slot
void CompositeTileSourceConfigurationWidget::handleCompositeChange()
{
    QItemSelectionModel * selModel = this->ui->listView->selectionModel();
    QModelIndex index = selModel->currentIndex();

    QSharedPointer<CompositeTileSource> strong = _composite.toStrongRef();
    if (strong.isNull())
        return;

    qreal opacityFloat = strong->getOpacity(index.row());
    this->ui->opacitySlider->setValue(opacityFloat*100);
}

//private slot
void CompositeTileSourceConfigurationWidget::on_removeSourceButton_clicked()
{
    QItemSelectionModel * selModel = this->ui->listView->selectionModel();
    QModelIndex index = selModel->currentIndex();

    QSharedPointer<CompositeTileSource> strong = _composite.toStrongRef();
    if (strong.isNull())
        return;

    strong->removeSource(index.row());
}

//private slot
void CompositeTileSourceConfigurationWidget::on_opacitySlider_valueChanged(int value)
{
    QItemSelectionModel * selModel = this->ui->listView->selectionModel();
    QModelIndex index = selModel->currentIndex();

    if (!index.isValid())
        return;

    QSharedPointer<CompositeTileSource> strong = _composite.toStrongRef();
    if (strong.isNull())
        return;

    qreal opacityFloat = (qreal)value / 100.0;

    strong->setOpacity(index.row(),opacityFloat);
}

//private slot
void CompositeTileSourceConfigurationWidget::on_moveDownButton_clicked()
{
    QItemSelectionModel * selModel = this->ui->listView->selectionModel();
    QModelIndex index = selModel->currentIndex();

    if (!index.isValid())
        return;

    QSharedPointer<CompositeTileSource> strong = _composite.toStrongRef();
    if (strong.isNull())
        return;

    int numberOfLayers = strong->numSources();
    int currentIndex = index.row();
    int desiredIndex = qMin<int>(numberOfLayers-1,currentIndex+1);
    strong->moveSource(currentIndex,desiredIndex);
}

//private slot
void CompositeTileSourceConfigurationWidget::on_moveUpButton_clicked()
{
    QItemSelectionModel * selModel = this->ui->listView->selectionModel();
    QModelIndex index = selModel->currentIndex();

    if (!index.isValid())
        return;

    QSharedPointer<CompositeTileSource> strong = _composite.toStrongRef();
    if (strong.isNull())
        return;

    int currentIndex = index.row();
    int desiredIndex = qMax<int>(0,currentIndex-1);
    strong->moveSource(currentIndex,desiredIndex);
}
