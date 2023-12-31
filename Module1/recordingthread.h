#ifndef RECORDINGTHREAD_H
#define RECORDINGTHREAD_H

// #include <stdio.h>
#include <QThread>
#include <QRunnable>
#include <vtkRenderWindowInteractor.h>
#include <Windows.h>
#include <windowsx.h>
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
    recordingThread(int type);
    void startRecording(QString);
    
public slots:
    void stopRecording();
    void finishRecording();
protected:
    void run() override;
private:
    HANDLE hThread;
    // 0:keyboard  1:mouse
    int type; 
signals:
    void sendNewAction(QString);
    void sendNewRecord(QString,int type);
};

#endif // RECORDINGTHREAD_H


extern recordingThread* keyboardThread;
extern recordingThread* mouseThread;

extern qint64 delta_time;
// old_sys_time - cmd_time = old_delta_time
// new_sys_time - cmd_time = new_delta_time
qint64 getDeltaTime();
void setDeltaTime(qint64 time);

void SaveBMPFile(const char* filename, HBITMAP hBitmap, HDC hdcMem, int width, int height);
void CaptureScreenAndSave(const char* filename);