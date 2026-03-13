#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "texttools.h"
#include <QFileDialog>
#include <QTextStream>
#include <QFileInfo>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentHighlighter(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle("Notatnik");

    TextTools::setupLineCounterUI(ui->noteText, ui->lineCounter);
    connect(ui->noteText, &QPlainTextEdit::textChanged, this, [this]() {
        TextTools::updateLineCounter(ui->noteText, ui->lineCounter);
    });

    // Load built-in grammars from resources, then user grammars on top
    grammarLoader.loadFromDirectory(QDir::currentPath() + "/syntax");
    qDebug() << "Looking for grammars in:" << QDir::currentPath() + "/syntax";
    qDebug() << "Languages found:" << grammarLoader.availableLanguages();
}

MainWindow::~MainWindow()
{
    delete currentHighlighter;
    delete ui;
}

void MainWindow::setTitle(QString title){
    if(title.isEmpty()) return;
    this->setWindowTitle(title + " - Notatnik");
}

void MainWindow::applyHighlighter(const QString& filePath)
{
    delete currentHighlighter;
    currentHighlighter = nullptr;

    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();

    QJsonObject grammar = grammarLoader.grammarForExtension(ext);
    if(!grammar.isEmpty()){
        currentHighlighter = new SyntaxHighlighter(ui->noteText->document());
        currentHighlighter->loadFromJson(grammar);
    }
}

void MainWindow::on_openFile_triggered()
{
    QString filePath = FileHandling::getOpenFilePath(this);
    if(filePath.isEmpty()) return;

    QString content = FileHandling::openFile(filePath);
    if(!content.isNull()){
        ui->noteText->setPlainText(content);
        currentFilePath = filePath;
        setTitle(filePath);
        applyHighlighter(filePath);
    }
}

void MainWindow::on_createFile_triggered()
{
    delete currentHighlighter;
    currentHighlighter = nullptr;

    ui->noteText->clear();
    currentFilePath = QString();
    this->setWindowTitle("Notatnik");
}

void MainWindow::on_saveFile_triggered()
{
    if(currentFilePath.isEmpty()){
        on_saveFileAs_triggered();
    } else {
        QFile file(currentFilePath);
        if(file.open(QFile::WriteOnly | QFile::Text)){
            QTextStream out(&file);
            out << ui->noteText->toPlainText();
            file.close();
        }
    }
}

void MainWindow::on_saveFileAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz jako", "", "Pliki tekstowe (*.txt);;Wszystkie pliki (*)");

    if(!fileName.isEmpty()){
        currentFilePath = fileName;

        QFile file(fileName);
        if(file.open(QFile::WriteOnly | QFile::Text)){
            QTextStream out(&file);
            out << ui->noteText->toPlainText();
            setTitle(currentFilePath);
            file.close();

            // Re-apply since extension may have changed
            applyHighlighter(currentFilePath);
        }
    }
}