#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <QString>
#include <QWidget>
#include <QPlainTextEdit>

class FileHandling{
public:
    static QString openFile(const QString &filePath);
    static bool saveFile(const QString &filePath, const QString &content);

    static QString getOpenFilePath(QWidget *parent);
    static QString getSaveFilePath(QWidget *parent);

    enum class SaveResult {
        NoChanges,      // Nic nie zmieniono, działaj dalej
        SaveRequested,  // Użytkownik chce zapisać
        DiscardChanges, // Użytkownik chce odrzucić zmiany
        CancelAction    // Użytkownik kliknął Anuluj - stój!
    };

    static SaveResult checkSaveStatus(QWidget *parent, QPlainTextEdit *editor);
};

#endif // FILEHANDLING_H
