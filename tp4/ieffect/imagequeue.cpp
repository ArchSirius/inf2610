#include <QImage>
#include <QDebug>

#include "imagequeue.h"
#include "simpletracer.h"

ImageQueue::ImageQueue(QObject *parent, int capacity) :
    QObject(parent),
    m_capacity(capacity)
{
    pSemaphore = CreateSemaphore(
                NULL,
                capacity,
                capacity,
                NULL);

    cSemaphore = CreateSemaphore(
                NULL,
                0,
                capacity,
                NULL);

    hMutex = CreateMutex(
                NULL,
                FALSE,
                NULL);
}

ImageQueue::~ImageQueue()
{
   CloseHandle(pSemaphore);
   CloseHandle(cSemaphore);
   CloseHandle(hMutex);
}

void ImageQueue::enqueue(QImage *item)
{
    // P()
    WaitForSingleObject(pSemaphore, INFINITE);
    WaitForSingleObject(hMutex, INFINITE);

    m_queue.enqueue(item);    
    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);

    ReleaseMutex(hMutex);
    ReleaseSemaphore(cSemaphore, 1, NULL);
}

QImage *ImageQueue::dequeue()
{
    WaitForSingleObject(cSemaphore, INFINITE);
    WaitForSingleObject(hMutex, INFINITE);

    QImage* item = m_queue.dequeue();
    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);

    ReleaseMutex(hMutex);
    // V()
    ReleaseSemaphore(pSemaphore, 1, NULL);

    return item;
}
