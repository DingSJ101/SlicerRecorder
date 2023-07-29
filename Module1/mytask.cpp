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
qint64 last_time = 0;
bool startReproduce = false;

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
    keyboardTask->start();
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
    qint64  currentTime = QDateTime::currentMSecsSinceEpoch();
    // TODO : 同步启动reproduce部分，目前keyboard需要预处理数据
    addRecord(QString::number(currentTime)+":START:0",0);
    addRecord(QString::number(currentTime)+":0:0:0",1);
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
    HANDLE hKeyboardThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size
        keyboaredReproduceThread,             // thread function name
        (LPVOID)keyboard_filename,       // Pass the filename as a parameter to the thread
        0,                      // use default creation flags
        NULL               // returns the thread identifier
    );
    HANDLE hMouseThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size
        mouseReproduceThread,             // thread function name
        (LPVOID)mouse_filename,       // Pass the filename as a parameter to the thread
        0,                      // use default creation flags
        NULL               // returns the thread identifier
    );

    FILE *fp = fopen(keyboard_filename,"r");
    // 从文件中读入time
    char line[1024];
    qint64 time;
    char action[10];
    char info[10];
    fscanf(fp,"%lld:%[^:]:%s",&time,action,info);
    fclose(fp);
    setDeltaTime(QDateTime::currentMSecsSinceEpoch()-time+1000);
    // if (hKeyboardThread == NULL)
    // {
    //     emit echoInfoImp("Fail to create thread");
    //     return;
    // }
    // else
    // {
    //     emit echoInfoImp("Start reproducing");
    // }
    emit echoInfoImp("[out] myTask::reproduce()");
}

// TODO : 对于重复按键，其行为为第一次按键后，延迟500ms触发第二次按键，之后30ms触发一次
// 如果简化了这部分的重复按键，需要在之后还原
DWORD WINAPI keyboaredReproduceThread(LPVOID lpParam)
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
    // Sleep(1000); // TODO : 将窗口置顶、并移除鼠标焦点
    emit myTask_ptr->addAction(QString("del_cnt = ")+QString::number(cnt)+QString(" tol = ")+QString::number(tol));
    fclose(fp);
    fclose(newfp);
    fp = fopen(newfilename,"r");
    // qint64 delta_time = 0;
    DWORD vkey = 0;
    int key_action = 0 ;
    while(fgets(line,1024,fp)){
        sscanf_s(line,"%lld:%[^:]:%s",&time,action,10,info,10);
        // if (delta_time == 0) {
        //     delta_time = QDateTime::currentMSecsSinceEpoch()-time;
        // }
        qint64 local_delta_time = getDeltaTime();
        vkey = atoi(info);
        if (strcmp(action,"DOWN")==0)
        {
            key_action = 0;
        }
        else if (strcmp(action,"UP")==0)
        {
            key_action = KEYEVENTF_KEYUP;
            // if (vkey == VK_F11){
            //     CaptureScreenAndSave("D:\\test.png");
            // }
        }
        else
        {
            qDebug()<<"unknown :"<<line;
        }
        // int cnt = 0;
        while(local_delta_time<0 || QDateTime::currentMSecsSinceEpoch() <= time+ local_delta_time)
        {
            // cnt++;
            Sleep(10);
            local_delta_time = getDeltaTime();
        }
        keybd_event(vkey,0,key_action,0);
        // qDebug()<<vkey<<cnt;
        // qDebug()<<delta_time<<time<<QDateTime::currentMSecsSinceEpoch()<<QDateTime::currentMSecsSinceEpoch()-time;
    }
    fclose(fp);
    fp=nullptr;
    return 0;
}

DWORD WINAPI mouseReproduceThread(LPVOID lpParam)
{
    emit myTask_ptr->addAction(QString("[in] mouseReproduceThread()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    const char* filename = (const char*)lpParam;
    FILE *fp = fopen(filename,"r");

    char line[1024];
    qint64 time;
    char action[10];
    char param1[10];
    char param2[10];
    // Sleep(1000); // TODO : 将窗口置顶、并移除鼠标焦点
    // qint64 delta_time = 0;
    DWORD vkey = 0;
    int key_action = 0 ;
    int o_x=-1,o_y=-1;
    qint64 local_delta_time = getDeltaTime();
    // while(local_delta_time<0)Sleep(10),local_delta_time = getDeltaTime();
    while(fgets(line,1024,fp)){
        int local_delta_time = getDeltaTime();
        // line is in format like 1690209042355:512:483:460
        sscanf_s(line,"%lld:%[^:]:%[^:]:%s",&time,action,10,param1,10,param2,10);
        // if (delta_time == 0) {
        //     delta_time = QDateTime::currentMSecsSinceEpoch()-time;
        // }
        vkey = atoi(action);
        int event,x=0,y=0,z=0,_x,_y;
        switch (vkey)
        {
            case WM_MOUSEMOVE:
                _x = atoi(param1);
                _y = atoi(param2);
                x = _x - o_x;
                y = _y - o_y;
                o_x = _x;
                o_y = _y;
                if(o_x==-1 || o_y==-1){
                    // SetCursorPos(_x,_y);
                    // emit myTask_ptr->addAction(QString::number(_x)+QString(",")+QString::number(_y));
                }
                event = MOUSEEVENTF_MOVE;
                // event = MOUSE_MOVE_ABSOLUTE;
                // event = MOUSEEVENTF_ABSOLUTE;
                // x = _x;
                // y = _y;
                emit myTask_ptr->addAction(QString::number(x)+QString(",")+QString::number(y)+QString(" ")+QString::number(_x)+QString(",")+QString::number(_y));
                break;
            case WM_LBUTTONDOWN:
                event = MOUSEEVENTF_LEFTDOWN;
                break;
            case WM_RBUTTONDOWN:
                event = MOUSEEVENTF_RIGHTDOWN;
                break;
            case WM_MBUTTONDOWN:
                event = MOUSEEVENTF_MIDDLEDOWN;
                break;
            case WM_LBUTTONUP:
                event = MOUSEEVENTF_LEFTUP;
                break; 
            case WM_RBUTTONUP:
                event = MOUSEEVENTF_RIGHTUP;
                break;
            case WM_MBUTTONUP:
                event = MOUSEEVENTF_MIDDLEUP;
                break;
            case WM_XBUTTONUP:
                event = MOUSEEVENTF_XUP;
                break;
            case WM_MOUSEWHEEL:
                z = atoi(param1);
                event = MOUSEEVENTF_WHEEL;
                break;
            case WM_MOUSEHWHEEL:
                z = atoi(param1);
                event = MOUSEEVENTF_HWHEEL;
                break;
                
        }
        // int cnt = 0;
        while(local_delta_time<0 || QDateTime::currentMSecsSinceEpoch() <= time+ local_delta_time)
        {
            // cnt++;
            Sleep(10);
            local_delta_time = getDeltaTime();
        }
        // mouse_event(event,x,y,z,0);

        // qDebug()<<vkey<<cnt;
        // qDebug()<<delta_time<<time<<QDateTime::currentMSecsSinceEpoch()<<QDateTime::currentMSecsSinceEpoch()-time;
        if(event ==MOUSEEVENTF_MOVE) {SetCursorPos(_x,_y);continue;}
        MOUSEINPUT mi = {x,y,z,event};
        INPUT input = {INPUT_MOUSE,mi};
        SendInput(1, &input, sizeof(input));
        POINT pt ;
        GetCursorPos(&pt);
        myTask_ptr->addAction(QString("after move ")+QString::number(pt.x)+QString(",")+QString::number(pt.y));
        
    }
    fclose(fp);
    fp=nullptr;
    return 0;
}