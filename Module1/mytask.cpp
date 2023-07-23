#include "mytask.h"
#include <QTime>

myTask::myTask(QObject* parent)
{
    qDebug() <<"myTask:myTask()"<< this->thread()->currentThreadId();
    myTask_ptr = this;
}

myTask* myTask_ptr = nullptr;
recordingThread* keyboardThread = nullptr;
recordingThread* mouseThread = nullptr;

void myTask::initialize()
{
    qDebug()<<"myTask::initialize()"<<this->thread()->currentThreadId();
    emit echoInfoImp(QString("[in] myTask::initialize()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));

    if (keyboardTask == nullptr)
    {
        keyboardTask = new recordingThread(0);
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
        mouseTask = new recordingThread(1);
        mouseThread = mouseTask;
        emit echoInfoImp("  [new] mouseTask = new recordingThread()");

        connect(mouseTask,&recordingThread::sendNewAction,this,&myTask::addAction);
        connect(mouseTask,&recordingThread::sendNewRecord,this,&myTask::addRecord);

        connect(mouseTask,&QThread::started,[this]{emit echoInfoImp("mouseTask -> QThread::started");});
        connect(mouseTask,&QThread::finished,[this]{emit echoInfoImp("mouseTask -> QThread::finish");});
        connect(mouseTask,&QThread::destroyed,[this]{emit echoInfoImp("mouseTask -> QThread::destory");});

        connect(this,&myTask::stopThread,mouseTask,&recordingThread::stopRecording);
    }
    else{
        emit echoInfoImp("  [error] mouseTask exists");
        return ;
    }


    //  QThread
    // keyboardTask->start();
    mouseTask->start();

    //  QThreadPool
//    QThreadPool *ThreadPool = QThreadPool::globalInstance();
//    ThreadPool->setMaxThreadCount(10);
//    ThreadPool->start(mouseTask);
//    ThreadPool->start(keyboardTask);
    //

    
    // if (keyboardTask->isFinished())    emit echoInfoImp("keyboardTask is Finished");
    // if (keyboardTask->isRunning())    emit echoInfoImp("keyboardTask is Running");
    // fopen_s(&fp_keyboard,filename,"w");
    fp_keyboard = fopen(keyboard_filename,"w");
    emit echoInfoImp("  [open] fp_keyboard = fopen(keyboard_filename,\"w\")");
    fp_mouse = fopen(mouse_filename,"w");
    emit echoInfoImp("  [open] fp_mouse = fopen(mouse_filename,\"w\")");
    emit echoInfoImp("[out] myTask::initialize()");
}

void myTask::addAction(QString info)
{
    // qDebug()<<"myTask::addAction("<<info<<")";
    emit echoInfoImp(info);
}

void myTask::addKeyboardRecord(QString info)
{
    fprintf(fp_keyboard,"%s\n",info.toStdString().c_str());
}
void myTask::addMouseRecord(QString info)
{
    fprintf(fp_mouse,"%s\n",info.toStdString().c_str());
}
void myTask::addRecord(QString info,int task_type)
{
    qDebug()<<"myTask::addRecord("<<info<<")";
    if (task_type == 0) addKeyboardRecord(info);
    else if (task_type == 1) addMouseRecord(info);
}

void myTask::finish()
{
    // qDebug()<<"myTask::finish()";
    emit echoInfoImp(QString("[in] myTask::finish()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    // if (keyboardTask->isFinished())    emit echoInfoImp("keyboardTask is Finished");
    // if (keyboardTask->isRunning())    emit echoInfoImp("keyboardTask is Finished");
    emit stopThread();
    
    
    // if (keyboardTask && keyboardTask->isFinished())    emit echoInfoImp("keyboardTask is Finished after stopThread()");
    // if (keyboardTask && keyboardTask->isRunning())    emit echoInfoImp("keyboardTask is Finished after stopThread()");
    
    fclose(fp_keyboard);
    fp_keyboard = nullptr;
    fclose(fp_mouse);
    fp_mouse = nullptr;
    emit echoInfoImp("  [close] fp_keyboard = nullptr");
    emit echoInfoImp("  [close] fp_mouse = nullptr");
    emit echoInfoImp("[out] myTask::finish()");
}

// void myTask::simplify()
// {
//     emit echoInfoImp(QString("[in] myTask::simplify()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
//     fp = fopen(filename,"r");
//     emit echoInfoImp("  [open] fp = fopen(filename,\"r\")");
//     FILE *newfp = fopen(newfilename,"w");
//     emit echoInfoImp("  [open] newfp = fopen(newfilename,\"w\")");

//     // read each line of the file
//     char line[1024];
//     // each line is formatted with time:action:info
//     qint64 time;
//     char action[10];
//     char info[10];
//     while(fgets(line,1024,fp)){
//         qint64 new_time;
//         char new_action[10];
//         char new_info[10];

//         sscanf_s(line,"%lld:%[^:]:%s",&new_time,new_action,10,new_info,10);
//         // if new_action and new_info is the same as action and info, then time = new_time
//         if ( 
//                 strcmp(info,new_info) != 0 
//                 // && strcmp("UP",new_action) != 0 
//                 || (strcmp(new_action,"UP")==0 ||strcmp(action,"UP")==0)
//             )
//         {
//             // fwrite(line,1,strlen(line),newfp);
//             fprintf(newfp,"%s",line);
//             time = new_time;
//             strcpy_s(action,10,new_action);
//             strcpy_s(info,10,new_info);
//         }
//         else
//         {
//             qDebug()<<line;
//             continue;
//         }
//     }
//     fclose(fp);
//     fp=nullptr;
//     fclose(newfp);
//     newfp=nullptr;
//     emit echoInfoImp("  [close] fp = nullptr");
//     emit echoInfoImp("  [close] newfp = nullptr");
//     emit echoInfoImp("[out] myTask::simplify()");
// }

void myTask::reproduce(){
    emit echoInfoImp(QString("[in] myTask::reproduce()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    // Pass the filename as a parameter to the thread
    HANDLE hThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size
        reproduceThread,             // thread function name
        (LPVOID)keyboard_filename,       // Pass the filename as a parameter to the thread
        0,                      // use default creation flags
        NULL               // returns the thread identifier
    );
    if (hThread == NULL)
    {
        emit echoInfoImp("Fail to create thread");
        return;
    }
    else
    {
        emit echoInfoImp("Start reproducing");
    }
    emit echoInfoImp("[out] myTask::reproduce()");
}
// TODO : 对于重复按键，其行为为第一次按键后，延迟500ms触发第二次按键，之后30ms触发一次
// 如果简化了这部分的重复按键，需要在之后还原
DWORD WINAPI reproduceThread(LPVOID lpParam)
{
    emit myTask_ptr->addAction(QString("[in] reproduceThread()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    const char* filename = (const char*)lpParam;
    const char* newfilename = "D:\\text_new.txt";

    FILE *fp = fopen(filename,"r");
    FILE *newfp = fopen(newfilename,"w");

    char line[1024];
    qint64 time;
    char action[10];
    char info[10];
    int cnt = 0;
    int tol = 0;
    Sleep(1000); // TODO : 将窗口置顶、并移除鼠标焦点
    while(fgets(line,1024,fp)){
        qint64 new_time;
        char new_action[10];
        char new_info[10];
        sscanf_s(line,"%lld:%[^:]:%s",&new_time,new_action,10,new_info,10);
        if ( 
                strcmp(info,new_info) != 0 
                || (strcmp(new_action,"UP")==0 ||strcmp(action,"UP")==0)
            )
        {
            fprintf(newfp,"%s",line);
            time = new_time;
            strcpy_s(action,10,new_action);
            strcpy_s(info,10,new_info);
            tol++;
        }
        else
        {
            cnt++;
        }
    }
    emit myTask_ptr->addAction(QString("del_cnt = ")+QString::number(cnt)+QString(" tol = ")+QString::number(tol));
    fclose(fp);
    fclose(newfp);
    fp = fopen(newfilename,"r");
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
            qDebug()<<"unknown :"<<line;
        }
        // int cnt = 0;
        while(QDateTime::currentMSecsSinceEpoch() <= time+ delta_time)
        {
            // cnt++;
            Sleep(10);
        }
        keybd_event(vkey,0,key_action,0);
        // qDebug()<<vkey<<cnt;
        // qDebug()<<delta_time<<time<<QDateTime::currentMSecsSinceEpoch()<<QDateTime::currentMSecsSinceEpoch()-time;
    }
    fclose(fp);
    fp=nullptr;
    return 0;
}