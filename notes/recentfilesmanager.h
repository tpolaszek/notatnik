#ifndef RECENTFILESMANAGER_H
#define RECENTFILESMANAGER_H
#include <QObject>
#include <QString>
#include <QList>
#include <QDateTime>

struct RecentFile {
    QString path;
    QDateTime lastModified;
};

class RecentFilesManager : public QObject
{
    Q_OBJECT;
public:
    explicit RecentFilesManager(QObject *parent = nullptr);

    void recordFile(const QString &filePath);

    QList<RecentFile> recentFiles() const;

    void load();
    void save() const;

private:
    static constexpr int MaxEntries = 10;
    QList<RecentFile> files;
};

#endif // RECENTFILESMANAGER_H
