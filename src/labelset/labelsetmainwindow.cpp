#include "labelsetmainwindow.h"
#include "ui_labelsetmainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QStandardPaths>

void LabelSetMainWindow::setImageView(int index = -1){
    if (index > -1){
        this->imageIndex = index;
    }
    ui->imageIndexView->setText(QString::number(this->imageIndex + 1));

    QString imagePath = this->datasetDirPath + "/" + this->imagePathList.at(this->imageIndex);
    QImage image(imagePath);
    if(image.isNull() == false){
        double imageWidth = image.width();
        double imageHeight = image.height();
        double mul = imageWidth / imageHeight;
        int viewWidth, viewHeight;


        qDebug() << mul;

        if (mul < 5.0 / 8.0){
            viewWidth = int(640 * mul);
            viewHeight = 640;
        }
        else{
            viewWidth = 400;
            viewHeight = int(400 / mul);
        }

        qDebug() << viewWidth << viewHeight;
        ui->imageView->resize(viewWidth, viewHeight);
        ui->imageSizeView->move(int(viewWidth / 2) - 120, 20 + viewHeight);
        ui->imageSizeView->setText(QString::number(imageWidth) + " × " + QString::number(imageHeight) +
                                   " (view size: " + QString::number(viewWidth) + " × " + QString::number(viewHeight) + ")");


        QPixmap pixmap(imagePath);
        ui->imageView->setPixmap(pixmap.scaled(ui->imageView->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else{
        errorMessageBox("The file is not an image");
    }
}

void LabelSetMainWindow::setLabelInputView(){
    ui->labelSetInput->setStyleSheet("color: black;");

    QString imagePath = this->imagePathList.at(this->imageIndex);
    if (this->datasetJsonObj.contains(imagePath)){
        ui->labelSetInput->setStyleSheet("color: red;");
        ui->labelSetInput->setText(QString::number(this->datasetJsonObj[imagePath].toInt()));
    }
    else{
        ui->labelSetInput->setText("");
    }
}

void LabelSetMainWindow::errorMessageBox(QString text){
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("error");
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void LabelSetMainWindow::dataJsonSumMessageBox(QJsonObject dataset){
    QString datasetSize = "dataset size: " + QString::number(dataset.size());

    int maxLabel = 0;
    QHash<int, int> labelNum;
    for (const QString &key : dataset.keys()) {
        int label = dataset[key].toInt();
        if (maxLabel < label){
            maxLabel = label;
        }


        if (!labelNum.contains(label)){
            labelNum[label] = 1;
        }
        else{
            labelNum[label] += 1;
        }
    }

    QString labels = "";
    for (int label = 1; label <= maxLabel;label++){
        labels += "label: " + QString::number(label) + ", num: " + QString::number(labelNum[label]) + "\n";
    }
    labels.chop(1);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("export");
    msgBox.setText(datasetSize + "\n" + labels);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

//select dataset dir path
void LabelSetMainWindow::datasetInitialize(QString datasetPath){
    if (datasetPath == ""){
        return;
    }

    QDir dataset(datasetPath);

    QFileInfoList entries = dataset.entryInfoList(QDir::Files);

    for (const QFileInfo &entryInfo : entries) {
        this->imagePathList.append(entryInfo.fileName());
    }

    if (this->imagePathList.length() > 0){
        this->imageIndex = 0;
        this->datasetDirPath = datasetPath;
        this->datasetJsonObj = QJsonObject();
        ui->datasetSelectDirPathShow->setText(datasetPath);
        ui->imageListLengthLabel->setText(QString::number(this->imagePathList.length()));

        setImageView();
        setLabelInputView();

        ui->datasetJsonFilePathView->setEnabled(true);
        ui->datasetJsonImport->setEnabled(true);
        ui->imageIndexView->setEnabled(true);
        ui->backImgSet->setEnabled(false);
        ui->nextImgSet->setEnabled(true);
        ui->labelSetInput->setEnabled(true);
        ui->labelSetSave->setEnabled(true);
        ui->unlabeledView->setEnabled(true);
        ui->exportDatasetJson->setEnabled(true);
    }
    else{
        errorMessageBox("The folder does not contain any files.");
    }
}

void LabelSetMainWindow::datasetJsonFileInitialize(QString datasetJsonFilePath){
    QFile file(datasetJsonFilePath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString fileContent = file.readAll();
        file.close();

        QJsonDocument datasetJsonDoc = QJsonDocument::fromJson(fileContent.toUtf8());
        if (datasetJsonDoc.isNull()) {
            errorMessageBox("Failed to parse JSON.");
            return;
        }

        if (!datasetJsonDoc.isObject()) {
            errorMessageBox("JSON is not an object.");
            return;
        }

        ui->datasetJsonFilePathView->setText(datasetJsonFilePath);
        this->datasetJsonObj = datasetJsonDoc.object();
        setLabelInputView();
    }
}

LabelSetMainWindow::LabelSetMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LabelSetMainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    this->setFixedSize(this->width(), this->height());

    ui->datasetJsonFilePathView->setEnabled(false);
    ui->datasetJsonImport->setEnabled(false);
    ui->imageIndexView->setEnabled(false);
    ui->backImgSet->setEnabled(false);
    ui->nextImgSet->setEnabled(false);
    ui->labelSetInput->setEnabled(false);
    ui->labelSetSave->setEnabled(false);
    ui->unlabeledView->setEnabled(false);
    ui->exportDatasetJson->setEnabled(false);

    //select dataset dir path
    connect(ui->datasetSelect, &QPushButton::clicked, this, [=](){
        QString datasetPath = QFileDialog::getExistingDirectory(this, "选择目录", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QFileDialog::ShowDirsOnly);
        datasetInitialize(datasetPath);
    });
    connect(ui->datasetSelectDirPathShow, &QLineEdit::returnPressed, this, [=](){
        QString datasetPath = ui->datasetSelectDirPathShow->text();
        datasetInitialize(datasetPath);
    });

    // import dataset's json(export)
    connect(ui->datasetJsonImport, &QPushButton::clicked, this, [=](){
        QString datasetJsonFilePath = QFileDialog::getOpenFileName(this, "选择文件", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), "Json files (*.json)");
        datasetJsonFileInitialize(datasetJsonFilePath);
    });
    connect(ui->datasetJsonFilePathView, &QLineEdit::returnPressed, this, [=](){
        QString datasetPath = ui->datasetJsonFilePathView->text();
        datasetInitialize(datasetPath);
    });

    connect(ui->backImgSet, &QPushButton::clicked, this, [=](){
        if (this->imageIndex > 0){
            this->imageIndex -= 1;
            setImageView();
            setLabelInputView();
            if (this->imageIndex < this->imagePathList.length()-1){
                ui->nextImgSet->setEnabled(true);
            }
            if (this->imageIndex == 0){
                ui->backImgSet->setEnabled(false);
            }
        }
    });
    connect(ui->nextImgSet, &QPushButton::clicked, this, [=](){
        if (this->imageIndex > -1 && this->imageIndex < this->imagePathList.length()-1){
            this->imageIndex += 1;
            setImageView();
            setLabelInputView();
            if(this->imageIndex > 0){
                ui->backImgSet->setEnabled(true);
            }
            if(this->imageIndex == this->imagePathList.length()-1){
                ui->nextImgSet->setEnabled(false);
            }
        }
    });
    connect(ui->imageIndexView, &QLineEdit::returnPressed, this, [=](){
        bool ok;
        int index = ui->imageIndexView->text().toInt(&ok);
        if (ok == false || index < 1 || index > this->imagePathList.length()){
            errorMessageBox("Invalid index.");
        }
        else{
            setImageView(index-1);
        }
    });

    connect(ui->labelSetSave, &QPushButton::clicked, this, [=](){
        if (this->imageIndex > -1){
            bool ok;
            int label = ui->labelSetInput->text().toInt(&ok);
            if (ok == false){
                errorMessageBox("Invalid label.");
            }
            else{
                this->datasetJsonObj[this->imagePathList.at(this->imageIndex)] = label;
                setLabelInputView();
            }
        }
    });

    connect(ui->unlabeledView, &QPushButton::clicked, this, [=](){
        QString unLabeled = "";

        for (int page = 0;page < this->imagePathList.length();page++){
            if (!this->datasetJsonObj.contains(this->imagePathList.at(page))){
                unLabeled += QString::number(page+1)+ ", ";
            }
        }

        if (unLabeled.length() == 0){
            unLabeled += "All data has been labeled.";
        }
        else{
            unLabeled.chop(2);
        }


        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("info");
        msgBox.setText(unLabeled);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    });

    connect(ui->exportDatasetJson, &QPushButton::clicked, this, [=](){
        QJsonDocument datasetJsonDoc(this->datasetJsonObj);
        QString datasetJsonString = datasetJsonDoc.toJson(QJsonDocument::Indented);

        QString datasetJsonSaveFilePath = QFileDialog::getSaveFileName(this, "保存文件", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), "Json files (*.json)");
        if (datasetJsonSaveFilePath == ""){
            return;
        }

        QFile datasetJsonFile(datasetJsonSaveFilePath);
        if (datasetJsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            datasetJsonFile.write(datasetJsonString.toUtf8());
            datasetJsonFile.close();
        }

        dataJsonSumMessageBox(this->datasetJsonObj);
    });
}

LabelSetMainWindow::~LabelSetMainWindow()
{
    delete ui;
}
