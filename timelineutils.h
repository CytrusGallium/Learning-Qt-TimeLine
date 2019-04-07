#ifndef TIMELINEUTILS_H
#define TIMELINEUTILS_H

#include "mainwindow.h"

class TimeLineEventMark : QObject
{
    // I don't think that class implementaion shall be in the header.
    private:
        QString name;
        int importance;
        QDateTime moment;
        QGraphicsScene *scene;
        QGraphicsEllipseItem *dot;
        QGraphicsTextItem *label;
        double secondsSinceYearZero;
    public:
        double GetSecondsSinceYearZero()
        {
            return secondsSinceYearZero;
        }

        TimeLineEventMark(QString ParamName, int ParamImportance, QDateTime ParamMoment, QGraphicsScene *ParamScene)
        {
            name = ParamName;
            importance = ParamImportance;
            moment = ParamMoment;
            scene = ParamScene;

            secondsSinceYearZero = moment.date().year() * 364.75 * 24 * 3600
                                 + moment.date().month() * 30 * 24 * 3600
                                 + moment.date().day() * 24 * 3600
                                 + moment.time().hour() * 3600
                                 + moment.time().minute() * 60
                                 + moment.time().second();

            int min = -300;
            int max = +300;
            int randomY = rand() % min + max;

            dot = ParamScene->addEllipse(secondsSinceYearZero/10000000,randomY,8,8);
            label = ParamScene->addText(ParamName);
            label->setX(secondsSinceYearZero/10000000);
            label->setY(randomY);

        }
};



/* You can define/implement constructors just like c#, but i think it's preferably done in the source instead of the header
TimeLineEventMark::TimeLineEventMark(QString ParamName, int ParamImportance, QDateTime ParamMoment, QGraphicsScene *ParamScene)
{

}
*/

// Shall be in a header
class TimeLineGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit TimeLineGraphicsScene(QObject *parent = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
};

// Shall be in the source
void TimeLineGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "in";
    if (mouseEvent->button() == Qt::LeftButton)
    {
        QGraphicsItem *item = itemAt(mouseEvent->scenePos(), QTransform());// it is your clicked item, you can do everything what you want. for example send it somewhere
        QGraphicsEllipseItem *ell = qgraphicsitem_cast<QGraphicsEllipseItem *>(item);
        if(ell)
        {
            ell->setBrush(QBrush(Qt::black));
        }
        else
            qDebug() << "not ell" << mouseEvent->scenePos();
        }

}

#endif // TIMELINEUTILS_H
