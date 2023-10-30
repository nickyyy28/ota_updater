#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DecodeThread.h"
#include "SerialDecoder.h"
#include <memory>
#include <qcombobox.h>
#include <qdebug.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <QDebug>
#include <QFileDialog>
#include <qserialportinfo.h>
#include <qtimer.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serial_opened(false)
{
    ui->setupUi(this);
    ui->firmware_write_progress->setMinimum(0);
    ui->firmware_write_progress->setMaximum(100);
    ui->firmware_write_progress->setValue(0);

    ui->combo_boundrate->setEditable(false);
    ui->combo_select_serial->setEditable(false);

    m_decoder = std::make_shared<ota_client::SerialDecoder>(this);

    connect(ui->btn_write_firmware, &QPushButton::clicked, [this](){
        qDebug() << "hello";
    });

    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(1000);
    connect(m_scanTimer, &QTimer::timeout, [this](){
        QString currentPort = ui->combo_select_serial->currentText();
        ui->combo_select_serial->clear();
        int idx = 0;
        for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
            ui->combo_select_serial->addItem(info.portName());
            if (info.portName() == currentPort) {
                // qDebug() << "index: " << idx << " port name: " << info.portName();
                ui->combo_select_serial->setCurrentIndex(idx);
            }
            idx ++;
        }
    });

    m_scanTimer->start();

    QStringList baudrateList;
    baudrateList << "110" << "300" << "600" << "1200" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400" << "43000" << "57600"
                    << "76800" << "115200" << "128000" << "230400" << "256000" << "460800" << "921600" << "1000000" << "2000000" << "3000000";

    ui->combo_boundrate->addItems(baudrateList);
    ui->combo_boundrate->setCurrentIndex(13);

    m_decoder->setBaudRate(115200);

    ui->lineEdit->setReadOnly(true);

    connect(ui->combo_boundrate, &QComboBox::currentTextChanged, [this](const QString& baudrate){
        m_decoder->setBaudRate(ui->combo_boundrate->currentText().toInt());
    });

    connect(ui->combo_select_serial, &QComboBox::currentTextChanged, [this](){
        m_decoder->setPort(ui->combo_select_serial->currentText());
    });

    connect(ui->btn_open_serial, &QPushButton::clicked, this, &MainWindow::tryOpen);

    connect(this, &MainWindow::openSucceeded, [this](){
        ui->btn_open_serial->setText("关闭串口");
        ui->btn_write_firmware->setEnabled(true);
    });

    connect(this, &MainWindow::openFailed, [this](){
        ui->btn_open_serial->setText("打开串口");
        ui->btn_write_firmware->setEnabled(false);
    });

    connect(ui->btn_read_firmware_info, &QPushButton::clicked, [this](){
        // m_decoder->testPost();
        // qDebug() << "aaaaaaaa";
    });

    connect(ui->btn_set_firmware_path, &QPushButton::clicked, [this](){
        // QFileDialog *dialog = new QFileDialog(this);
        // dialog->setWindowTitle("固件路径");
        // dialog->setFileMode(QFileDialog::AnyFile);
        // dialog->setViewMode(QFileDialog::Detail);
        // dialog->setFilter(tr("Firmware Files(*.bin)"));
        QString firmware_name = QFileDialog::getOpenFileName(this, tr("固件路径"), "C:/", tr("bin files(*.bin);;All files(*.*)"));
        if (firmware_name.isEmpty()) {
            QMessageBox::warning(this, "Warning!", "File Not Exist!");
        } else {
            this->ui->lineEdit->setText(firmware_name);
        }
    });

    m_decode_thread = new ota_client::DecodeThread(this, m_decoder);
    m_decode_thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tryOpen()
{
    if (!m_serial_opened) {
        bool ret = m_decoder->open();
        if (ret) {
            emit openSucceeded();
            m_serial_opened = true;
        } else {
            emit openFailed();
            m_serial_opened = false;
        }
    } else {
        m_decoder->close();
        emit openFailed();
        m_serial_opened = false;
    }
}
