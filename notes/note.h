#include <QString>
#ifndef NOTE_H
#define NOTE_H
enum class NoteType{
    txt,
    markdown,
    batch
};
class Note{
public:
    Note(QString name, QString path, NoteType type);
    QString getFullPath() const;

    bool saveState(const QString &content);
private:
    QString mName;
    QString mLocation;
    QString mType;
};

#endif // NOTE_H
