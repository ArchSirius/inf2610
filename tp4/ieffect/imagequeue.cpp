#include <QImage>
#include <QDebug>

#include "imagequeue.h"
#include "simpletracer.h"

ImageQueue::ImageQueue(QObject *parent, int capacity) :
    QObject(parent),
    m_capacity(capacity)
{
    hSemaphore = CreateSemaphore(
                NULL,
                capacity,
                capacity,
                NULL);
    hMutex = CreateSemaphore(
                NULL,
                1,
                1,
                NULL);
}

ImageQueue::~ImageQueue()
{
}

void ImageQueue::enqueue(QImage *item)
{
    // P()
    WaitForSingleObject(hSemaphore, INFINITE);

    WaitForSingleObject(hMutex, INFINITE);
    m_queue.enqueue(item);
    ReleaseSemaphore(hMutex, 1, NULL);

    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);
}

QImage *ImageQueue::dequeue()
{
    //if (m_queue.isEmpty())
        //return NULL;

    WaitForSingleObject(hMutex, INFINITE);
    QImage* item = m_queue.dequeue();
    ReleaseSemaphore(hMutex, 1, NULL);

    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);

    // V()
    ReleaseSemaphore(hSemaphore, 1, NULL);

    return item;
}
