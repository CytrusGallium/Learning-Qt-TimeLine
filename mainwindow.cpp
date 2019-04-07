#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "D:\C++\Test\Json Parser\WispJson\WispJson.h"
#include "timelineutils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(1);

    QGraphicsView *view = ui->timeLineGraphicsView;
    QGraphicsScene *scene = new QGraphicsScene(view);
    scene->setBackgroundBrush(Qt::lightGray);
    view->setScene(scene);

    //QDateTime eventDate(QDate(2012, 7, 6), QTime(8, 30, 0));
    //TimeLineEventMark mark("First Mark", 100, eventDate, scene);

    manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply)
    {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }

        // Get the Json Response as a VeriantMap
        QString answer = reply->readAll();

        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject jObject = doc.object();
        QVariantMap mainMap = jObject.toVariantMap();

        // Store and Check Type
        QString type = mainMap["Type"].toString();

        // qDebug() << mainMap["Entities"].toString();

        // Store session ID
        Session_ID = mainMap["Session_ID"].toString();

        if (type == "Login")
        {
            QString response = mainMap["Response"].toString();

            if (response == "UserNotFound")
            {
                msgBox.setText("User not found.");
                msgBox.exec();
            }
            else if (response == "IncorrectPassword")
            {
                msgBox.setText("Incorrect Password.");
                msgBox.exec();
            }
            else if (response == "Succes")
            {
                ui->tabWidget->removeTab(0);
                ui->tabWidget->addTab(ui->tabHome, "Home");
                GetTimeLines();
            }
        }
        else if (type == "EntityGrid" && lastHttpRequestIdentifier == "GetTimeLines")
        {
            map<string, string> json = ParseJson(answer.toLocal8Bit().constData());
            map<string, string> json_entities = ParseJson(json.at("Entities"));
            int count = json_entities.size();

            ui->tableTimeLine->setRowCount(count);
            ui->tableTimeLine->setColumnCount(1);
            QStringList labels;
            labels.insert(0, "Timeline(s)");
            ui->tableTimeLine->setHorizontalHeaderLabels(labels);

            for (int i = 1; i<=count; i++)
            {
                map<string, string> json_entities_solo = ParseJson(json_entities.at(to_string(i)));

                string s = json_entities_solo.at("PrimaryInfo");

                QString line = QString::fromStdString(s);
                QTableWidgetItem *pCell = ui->tableTimeLine->item(i-1, 0);
                if(!pCell)
                {
                    pCell = new QTableWidgetItem;
                    ui->tableTimeLine->setItem(i-1, 0, pCell);
                }
                pCell->setText(line);
            }


            //msgBox.setText(QString::fromStdString(s));
            //msgBox.exec();
        }
        else if (type == "EntityGrid" && lastHttpRequestIdentifier == "GetEvents")
        {
            //qDebug() << answer;

            ui->tabWidget->addTab(ui->tabTimeLine, "Timeline Viewer");

            map<string, string> json = ParseJson(answer.toLocal8Bit().constData());
            map<string, string> json_entities = ParseJson(json.at("Entities"));
            int count = json_entities.size();

            /*
            ui->tableTimeLine->setRowCount(count);
            ui->tableTimeLine->setColumnCount(1);
            QStringList labels;
            labels.insert(0, "Timeline(s)");
            ui->tableTimeLine->setHorizontalHeaderLabels(labels);
            */

            for (int i = 1; i<=count; i++)
            {
                map<string, string> json_entities_solo = ParseJson(json_entities.at(to_string(i)));

                string eventNameString = json_entities_solo.at("PrimaryInfo");
                string eventDateString = json_entities_solo.at("SecondaryInfo");
                string eventImportanceString = json_entities_solo.at("ThirdiaryInfo");

                /*
                qDebug() << QString::fromStdString(eventNameString);
                qDebug() << QString::fromStdString(eventDateString);
                qDebug() << QString::fromStdString(eventImportanceString);
                */

                QDate eventDateOnly = QDate::fromString(QString::fromStdString(eventDateString),"yyyy-MM-dd");

                QString eventName = QString::fromStdString(eventNameString);
                QDateTime eventDate(eventDateOnly, QTime(0, 0, 0));
                qDebug() << eventDate;

                TimeLineEventMark mark(eventName, 100, eventDate, scene);
            }
        }

        //qDebug() << answer;
    });


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOK_clicked()
{

    QString user = ui->edtUser->text();
    QString pass = ui->edtPass->text();

    lastHttpRequestIdentifier = "Login";
    request.setUrl(QUrl("http://127.0.0.1/TimeLine/View/WispLogin.php?user="+user+"&pass="+pass+"&r=json"));
    manager->get(request);

}

void MainWindow::GetTimeLines()
{

    lastHttpRequestIdentifier = "GetTimeLines";
    request.setUrl(QUrl("http://127.0.0.1/TimeLine/View/WispEntityGrid.php?entity=timeline"));
    manager->get(request);

}

void MainWindow::GetEvents()
{

    lastHttpRequestIdentifier = "GetEvents";
    request.setUrl(QUrl("http://127.0.0.1/Timeline/view/WispEntityGrid.php?entity=event&r=json"));
    manager->get(request);

}

void MainWindow::on_tableTimeLine_cellDoubleClicked(int row, int column)
{
    /*
    string sRow = to_string(row);
    string sCol = to_string(column);
    msgBox.setText("Cell Clicked at row " + QString::fromStdString(sRow) + " and column " + QString::fromStdString(sCol));
    msgBox.exec();
    */

    // qDebug() << ui->tableTimeLine->itemAt(row, column)->text();
    // ui->tabWidget->addTab(ui->tabTimeLine, "Timeline viewer");

    GetEvents();
}
