#ifndef LABELSETMAINWINDOW_H
#define LABELSETMAINWINDOW_H

#include <QMainWindow>

#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class LabelSetMainWindow;
}
QT_END_NAMESPACE

class LabelSetMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LabelSetMainWindow(QWidget *parent = nullptr);
    ~LabelSetMainWindow();

    void setImageView(int index);
    void setLabelInputView();

    void errorMessageBox(QString text);
    void dataJsonSumMessageBox(QJsonObject dataset);

    void datasetInitialize(QString datasetPath);
    void datasetJsonFileInitialize(QString datasetJsonFilePath);

    int imageIndex = -1;
    QString datasetDirPath;
    QList<QString> imagePathList;
    QJsonObject datasetJsonObj;

private:
    Ui::LabelSetMainWindow *ui;
};
#endif // LABELSETMAINWINDOW_H
