#include "settingsdialog.h"
#include "ui_settings.h"
#include "settingsmanager.h"
#include <QStackedWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QTimer>

SettingsDialog::SettingsDialog(SettingsManager *manager, ViewManager *viewManager, QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsWindow), manager(manager), viewManager(viewManager)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/settings.png"));

    chosenThemePath = manager->currentThemePath();
    ui->fontSizeBox->setValue(manager->fontSize());

    connect(ui->listWidget, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(ui->themeList, &QListWidget::itemClicked, this, &SettingsDialog::onThemeSelected);

    // podłączenie wszystkich przycisków do metod
    connect(ui->browseButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseClicked); // po kliknięciu przycisku "Przeglądaj" uaktywnia metode onBrowseClicked()
    connect(ui->pushButton, &QPushButton::clicked, this, &SettingsDialog::onApplyClicked); // po kliknięciu zastosuj uaktywnia metode onApplyClicked()
    connect(ui->pushButton_2, &QPushButton::clicked, this, &SettingsDialog::onOkClicked);  // po kliknięciu OK ukatywnia metode onOkClicked()

    populateThemeList();
    selectCurrentTheme();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

// Wypełnienie listy motywów plikami dostępnymi w /themes/ z rozszerzeniem .css
void SettingsDialog::populateThemeList() {
    ui->themeList->clear(); // czyszczenie listy

    // Dodaje każdy plik dostępny w folderze który jest odczytywany w metodzie availableThemes() w settingsmanager.cpp (14)
    for (const QString &path : manager->availableThemes()) {
        auto *item = new QListWidgetItem(QFileInfo(path).baseName());
        item->setData(Qt::UserRole, path);
        ui->themeList->addItem(item);
    }
}

// Pokazuje aktualnie używany motyw
void SettingsDialog::selectCurrentTheme() {
    const QString current = manager->currentThemePath();
    for (int i = 0; i < ui->themeList->count(); ++i) {
        if (ui->themeList->item(i)->data(Qt::UserRole).toString() == current) {
            ui->themeList->setCurrentRow(i);
            break;
        }
    }
}

// Po wybraniu motywu aplikuje go
void SettingsDialog::onThemeSelected(QListWidgetItem *item) {
    if (!item) return; // jeśli nie ma żadnych "przedmiotów" czyli elementów ListWidgeta nic nie pokazuje
    chosenThemePath = item->data(Qt::UserRole).toString();
}

// Po kliknięciu "Przeglądaj" otwiera okno dialogowe w którym wybiera się niestandardowy motyw do aplikacji
void SettingsDialog::onBrowseClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Otwórz niestandardowy motyw", QDir::homePath(), "Pliki CSS (*.css)");
    if (path.isEmpty()) return; // jeśli nie ma pliku nie dodawaj nic

    for (int i = 0; i < ui->themeList->count(); ++i) {
        if (ui->themeList->item(i)->data(Qt::UserRole).toString() == path) {
            ui->themeList->setCurrentRow(i);
            return;
        }
    }

    auto *item = new QListWidgetItem(QFileInfo(path).baseName() + " (niestandardowy)"); // dodaje niestandardowe motywy
    item->setData(Qt::UserRole, path);
    ui->themeList->addItem(item);
    ui->themeList->setCurrentItem(item); // ustawia ten motyw od razu po dodaniu go

    manager->setThemePath(path);
}

// Po kliknięciu zastosuj aplikuje zmienione ustawienia
void SettingsDialog::onApplyClicked() {
    manager->setThemePath(chosenThemePath);
    manager->setFontSize(ui->fontSizeBox->value());
    manager->save();
    manager->applyToApp(); // Aplikuje ustawienia

    int newSize = manager->fontSize();

    // Zmienia czcionke globalnie do wszystkich kart
    QFont font("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(newSize);

    for (QWidget *w : qApp->topLevelWidgets()) {
        for (QPlainTextEdit *e : w->findChildren<QPlainTextEdit*>()) {
            if (e->objectName() == "noteText" || e->objectName() == "lineCounter") {
                e->setFont(font);
            }
        }
    }

    // przetwarzanie zmieniania czcionki
    qApp->processEvents();

    // wymusza ustawienie rozmiaru czcionki i szerokośći licznika
    if (viewManager) {
        viewManager->applyFontSize(newSize);
    }
}
// Po kliknięciu OK zamyka okno i zapisuje zmiany
void SettingsDialog::onOkClicked() {
    onApplyClicked();
    accept();
}
