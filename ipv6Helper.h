#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <cstdint>
#include <QLabel>
#include <QLineEdit>


class IPv6Helper : public QMainWindow
{
    Q_OBJECT


public:
    explicit IPv6Helper(QWidget *parent = nullptr);

private slots:
    void onSubmitClicked();

private:
    QWidget *centralWidget;
    QLabel *applicationLabel;
    QLineEdit *m_inputArea;
    QLineEdit *m_cidrArea;
    QPushButton *m_submitButton;
    QTextEdit *m_outputArea;

};
