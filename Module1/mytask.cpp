#include "mytask.h"
#include <QTime>

myTask::myTask(QObject* parent)
{
    qDebug() <<"myTask:myTask()"<< this->thread()->currentThreadId();
}

recordingThread* keyboardThread = nullptr;

void myTask::initialize()
{
    qDebug()<<"myTask::initialize()"<<this->thread()->currentThreadId();
    emit echoInfoImp("[in] myTask::initialize()");
    if (keyboardTask == nullptr)
    {
        keyboardTask = new recordingThread();
        keyboardThread = keyboardTask;
        
        emit echoInfoImp("  [new] keyboardTask = new recordingThread()");
        
        connect(keyboardTask,&recordingThread::sendNewAction,this,&myTask::addAction);
        connect(keyboardTask,&recordingThread::sendNewRecord,this,&myTask::addRecord);
        
        // connect(keyboardTask,&QThread::finished,this,&myTask::finish);
        connect(keyboardTask,&QThread::started,[this]{emit echoInfoImp("keyboardTask -> QThread::started");});
        connect(keyboardTask,&QThread::finished,[this]{emit echoInfoImp("keyboardTask -> QThread::finish");});
        connect(keyboardTask,&QThread::destroyed,[this]{emit echoInfoImp("keyboardTask -> QThread::destory");});

        connect(this,&myTask::stopThread,keyboardTask,&recordingThread::stopRecording);
    }
    else{
        emit echoInfoImp("  [error] keyboardTask exists");
        return ;
    }
    if (mouseTask == nullptr)
    {
        mouseTask = new recordingThread();
        connect(mouseTask,&recordingThread::sendNewAction,this,&myTask::addRecord);
        connect(this,&myTask::stopThread,mouseTask,&recordingThread::finishRecording);
    }


    //  QThread
    keyboardTask->start();

    //  QThreadPool
//    QThreadPool *ThreadPool = QThreadPool::globalInstance();
//    ThreadPool->setMaxThreadCount(10);
//    ThreadPool->start(mouseTask);
//    ThreadPool->start(keyboardTask);
    //

    
    if (keyboardTask->isFinished())    emit echoInfoImp("keyboardTask is Finished");
    if (keyboardTask->isRunning())    emit echoInfoImp("keyboardTask is Running");
    // fopen_s(&fp,filename,"w");
    fp = fopen(filename,"w");
    emit echoInfoImp("  [open] fp = fopen(filename,\"w\")");
    emit echoInfoImp("[out] myTask::initialize()");
}

void myTask::addAction(QString info)
{
    // qDebug()<<"myTask::addAction("<<info<<")";
    emit echoInfoImp(info);
}

void myTask::addRecord(QString info)
{
    qDebug()<<"myTask::addRecord("<<info<<")";
    fprintf(fp,"%s\n",info.toStdString().c_str());
}

void myTask::finish()
{
    // qDebug()<<"myTask::finish()";
    emit echoInfoImp("[in] myTask::finish()");
    if (keyboardTask->isFinished())    emit echoInfoImp("keyboardTask is Finished");
    if (keyboardTask->isRunning())    emit echoInfoImp("keyboardTask is Finished");
    emit stopThread();
    
    // keyboardTask->quit();
    // keyboardTask->deleteLater();
    // emit echoInfoImp("keyboardTask->deleteLater()");
    if (keyboardTask && keyboardTask->isFinished())    emit echoInfoImp("keyboardTask is Finished after stopThread()");
    if (keyboardTask && keyboardTask->isRunning())    emit echoInfoImp("keyboardTask is Finished after stopThread()");
    
    fclose(fp);
    fp = nullptr;
    emit echoInfoImp("  [close] fp = nullptr");
    emit echoInfoImp("[out] myTask::finish()");
}

void myTask::simplify()
{
    qDebug()<<"myTask::simplify()";
    emit echoInfoImp("[in] myTask::simplify()");
    fp = fopen(filename,"r");
    emit echoInfoImp("  [open] fp = fopen(filename,\"r\")");
    FILE *newfp = fopen(newfilename,"w");
    emit echoInfoImp("  [open] newfp = fopen(newfilename,\"w\")");

    // read each line of the file
    char line[1024];
    // each line is formatted with time:action:info
    qint64 time;
    char action[10];
    char info[10];
    while(fgets(line,1024,fp)){
        qint64 new_time;
        char new_action[10];
        char new_info[10];

        sscanf_s(line,"%lld:%[^:]:%s",&new_time,new_action,10,new_info,10);
        // if new_action and new_info is the same as action and info, then time = new_time
        if ( 
                strcmp(info,new_info) != 0 
                // && strcmp("UP",new_action) != 0 
                || (strcmp(new_action,"UP")==0 ||strcmp(action,"UP")==0)
            )
        {
            // fwrite(line,1,strlen(line),newfp);
            fprintf(newfp,"%s",line);
            time = new_time;
            strcpy_s(action,10,new_action);
            strcpy_s(info,10,new_info);
        }
        else
        {
            qDebug()<<line;
            continue;
        }
    }
    fclose(fp);
    fp=nullptr;
    fclose(newfp);
    newfp=nullptr;
    emit echoInfoImp("  [close] fp = nullptr");
    emit echoInfoImp("  [close] newfp = nullptr");
    emit echoInfoImp("[out] myTask::simplify()");
}

void myTask::reproduce(){
    emit echoInfoImp("[in] myTask::reproduce()");
    if (fp == nullptr) {
        emit echoInfoImp("  [call] myTask::simplify()");
        simplify();
    }
    // read command in newfilename and reproduce it
    fp = fopen(newfilename,"r");
    emit echoInfoImp("  [open] fp = fopen(newfilename,\"r\")");
    
    char line[1024];
    // each line is formatted with time:action:info
    qint64 time;
    char action[10];
    char info[10];
    qint64 delta_time = 0;
    DWORD vkey = 0;
    int key_action = 0 ;
    while(fgets(line,1024,fp)){
        sscanf_s(line,"%lld:%[^:]:%s",&time,action,10,info,10);
        if (delta_time == 0) {
            delta_time = QDateTime::currentMSecsSinceEpoch()-time;
        }
        vkey = atoi(info);
        if (strcmp(action,"DOWN")==0)
        {
            key_action = 0;
        }
        else if (strcmp(action,"UP")==0)
        {
            key_action = KEYEVENTF_KEYUP;
        }
        else
        {
            qDebug()<<"unknown action:"<<action;
        }
        while(QDateTime::currentMSecsSinceEpoch() - time < delta_time)
        {
            msleep(1);
        }
        keybd_event(vkey,0,key_action,0);
        // qDebug()<<delta_time<<time<<QDateTime::currentMSecsSinceEpoch()<<QDateTime::currentMSecsSinceEpoch()-time;
    }
    fclose(fp);
    fp=nullptr;
    emit echoInfoImp("  [close] fp = nullptr");
    emit echoInfoImp("[out] myTask::reproduce()");
}