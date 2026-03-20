#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QFormLayout>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(SettingsManager *manager, QWidget *parent)
    : QDialog(parent), manager(manager)
{
    setWindowTitle("Settings");
    setMinimumWidth(400);
    buildUI();
    populateThemeList();
    selectCurrentTheme();
    fontSizeBox->setValue(manager->fontSize());
}

void SettingsDialog::buildUI() {
    auto *rootLayout = new QVBoxLayout(this);

    auto *tabs = new QTabWidget(this);

    auto *appearanceTab    = new QWidget;
    auto *appearanceLayout = new QVBoxLayout(appearanceTab);

    appearanceLayout->addWidget(new QLabel("Wszystkie motywy:"));

    themeList = new QListWidget;
    appearanceLayout->addWidget(themeList);

    auto *browseRow = new QHBoxLayout;
    previewLabel = new QLabel("Nie wybrano motywu");
    auto *browseButton = new QPushButton("Szukaj niestandardowych motywów..");
    browseRow->addWidget(previewLabel, 1);
    browseRow->addWidget(browseButton);
    appearanceLayout->addLayout(browseRow);

    tabs->addTab(appearanceTab, "Wygląd");

    auto *editorTab    = new QWidget;
    auto *editorLayout = new QFormLayout(editorTab);

    fontSizeBox = new QSpinBox;
    fontSizeBox->setRange(6, 72);
    editorLayout->addRow("Rozmiar czcionki (pt):", fontSizeBox);

    tabs->addTab(editorTab, "Edytor");

    rootLayout->addWidget(tabs);

    auto *buttonBox = new QDialogButtonBox;
    auto *applyBtn  = buttonBox->addButton("Zastosuj",  QDialogButtonBox::ApplyRole);
    auto *okBtn     = buttonBox->addButton("OK",     QDialogButtonBox::AcceptRole);
    auto *cancelBtn = buttonBox->addButton("Anuluj", QDialogButtonBox::RejectRole);
    rootLayout->addWidget(buttonBox);

    connect(themeList, &QListWidget::itemClicked,
            this, &SettingsDialog::onThemeSelected);
    connect(browseButton, &QPushButton::clicked,
            this, &SettingsDialog::onBrowseClicked);
    connect(applyBtn,  &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
    connect(okBtn,     &QPushButton::clicked, this, &SettingsDialog::onOkClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

// uzupełnia liste wszystkimi pliami .css w /themes/
void SettingsDialog::populateThemeList() {
    themeList->clear();
    for (const QString &path : manager->availableThemes()) {
        auto *item = new QListWidgetItem(QFileInfo(path).baseName());
        item->setData(Qt::UserRole, path);
        themeList->addItem(item);
    }
}

// Pokazuje który motyw jest teraz aktywny
void SettingsDialog::selectCurrentTheme() {
    const QString current = manager->currentThemePath();
    for (int i = 0; i < themeList->count(); ++i) {
        if (themeList->item(i)->data(Qt::UserRole).toString() == current) {
            themeList->setCurrentRow(i);
            previewLabel->setText(QFileInfo(current).baseName());
            break;
        }
    }
}

void SettingsDialog::onThemeSelected(QListWidgetItem *item) {
    QString path = item->data(Qt::UserRole).toString();
    manager->setThemePath(path);
    previewLabel->setText(QFileInfo(path).baseName());
}

// pozwala uzyc dowolnego pliku .css na komputerze
void SettingsDialog::onBrowseClicked() {
    QString path = QFileDialog::getOpenFileName(
        this, "Otwórz niestandardowy motyw", QDir::homePath(), "Pliki CSS (*.css)");
    if (path.isEmpty()) return;

    // Dodaj do listy jeśli nie ma żadnych
    for (int i = 0; i < themeList->count(); ++i)
        if (themeList->item(i)->data(Qt::UserRole).toString() == path) {
            themeList->setCurrentRow(i);
            return;
        }

    auto *item = new QListWidgetItem(QFileInfo(path).baseName() + " (niestandardowy)");
    item->setData(Qt::UserRole, path);
    themeList->addItem(item);
    themeList->setCurrentItem(item);

    manager->setThemePath(path);
    previewLabel->setText(QFileInfo(path).baseName());
}

// Zastosuj zmiany bez zamykania oka
void SettingsDialog::onApplyClicked() {
    manager->setFontSize(fontSizeBox->value());
    manager->save();
    manager->applyToApp();
}

// Zastosuj zmiany i zamknij okno
void SettingsDialog::onOkClicked() {
    onApplyClicked();
    accept();
}