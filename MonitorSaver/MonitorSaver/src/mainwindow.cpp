#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QPalette>
#include <QBrush>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), _backEnd(backEnd())
{
    ui->setupUi(this);

    connect(ui->save, &QPushButton::pressed, this, &MainWindow::onSave);
    connect(ui->load, &QPushButton::pressed, this, &MainWindow::onLoad);

    ui->saveLabel->setText(QString::fromStdString(_backEnd.getLastSaveDate()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSave() {

    std::string errorMessage = "";

    if (!_backEnd.save(errorMessage)) {
        ui->label->setText(QString::fromStdString(errorMessage));
    } else {
        ui->label->setText("Successful save");
    }

    ui->saveLabel->setText(QString::fromStdString(_backEnd.getLastSaveDate()));
}
void MainWindow::onLoad() {

    std::string errorMessage = "";

    if (!_backEnd.load(errorMessage)) {
        ui->label->setText(QString::fromStdString(errorMessage));
    } else {
        ui->label->setText("Successful load");
    }
}
