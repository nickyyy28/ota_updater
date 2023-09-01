#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <memory>
#include <qobjectdefs.h>
#include "DecodeThread.h"
#include "SerialDecoder.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void openSucceeded();
    void openFailed();

private slots:
    void tryOpen();

private:
    Ui::MainWindow *ui;

    QTimer *m_scanTimer;
    ota_client::SerialDecoder::ptr m_decoder;

    ota_client::DecodeThread *m_decode_thread;

    bool m_serial_opened;
};
#endif // MAINWINDOW_H
