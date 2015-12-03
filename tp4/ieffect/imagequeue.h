#ifndef IMAGEQUEUE_H
#define IMAGEQUEUE_H

#include <QObject>
#include <QQueue>
#include <windows.h>

class QImage;

class ImageQueue : public QObject
{
    Q_OBJECT
public:
    explicit ImageQueue(QObject *parent = 0, int capacity = 4);
    ~ImageQueue();
    void enqueue(QImage *item);
    QImage *dequeue();
    bool isEmpty();
private:
    int m_capacity;
    QQueue<QImage*> m_queue;
    HANDLE pSemaphore;
    HANDLE cSemaphore;
    HANDLE hMutex;
};

#endif // IMAGEQUEUE_H
