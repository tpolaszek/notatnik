#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <QString>
#include <QWidget>

class FileHandling{
public:
    static QString openFile(const QString &filePath);
    static bool saveFile(const QString &filePath, const QString &content);

    static QString getOpenFilePath(QWidget *parent);
    static QString getSaveFilePath(QWidget *parent);
};

#endif // FILEHANDLING_H
