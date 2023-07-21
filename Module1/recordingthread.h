#ifndef RECORDINGTHREAD_H
#define RECORDINGTHREAD_H

// #include <stdio.h>
#include <QThread>
#include <QRunnable>
#include <vtkRenderWindowInteractor.h>
#include <Windows.h>
#include <tchar.h>

class recordingThread
: public QThread,public QRunnable
{
    Q_OBJECT;

public:
    static recordingThread& GetInstance() {
        static recordingThread instance;
        return instance;
    }
    recordingThread();
    void startRecording(QString);
    
public slots:
    void stopRecording();
    void finishRecording();
protected:
    void run() override;
private:
    HANDLE hThread;
signals:
    void sendNewAction(QString);
    void sendNewRecord(QString);
};

#endif // RECORDINGTHREAD_H


extern recordingThread* keyboardThread;
