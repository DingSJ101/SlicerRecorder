#include "recordingthread.h"
#include <QDebug>
#include <QTime>
#include <vtkMRMLApplicationLogic.h>
recordingThread::recordingThread()
{
    qDebug() << "recordingThread::recordingThread()"<< this->thread()->currentThreadId();
    qDebug() << "recordingThread::recordingThread()"<< getpid();
//    this->quit();
}
void recordingThread::startRecording(QString word)
{
    emit sendNewAction(word);
    // msleep(100);
}
void recordingThread::stopRecording()
{
    emit sendNewAction(QString("[in] recordingThread::stopRecording()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    PostThreadMessage(GetThreadId(hThread), WM_QUIT, 0, 0);
    emit sendNewAction("[out] recordingThread::stopRecording()");
}
void recordingThread::finishRecording()
{
    qDebug()<<"recordingThread::finishRecording()";
    qDebug()<< this->thread()->currentThreadId();
//    this->thread()->quit();
}

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_MOUSEMOVE)
    {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        qDebug()<< "Mouse moved: X=" << cursorPos.x << ", Y=" << cursorPos.y;
//        std::ofstream logfile("mouse_log.txt", std::ios::app);
//        if (logfile.is_open())
//        {
//            logfile << "Mouse moved: X=" << cursorPos.x << ", Y=" << cursorPos.y << std::endl;
//            logfile.close();
//        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
    // recordingThread* eventProcessor = reinterpret_cast<recordingThread*>(GetProp(NULL, _T("MyEventProcessorInstance")));
    // qDebug()
    //     <<endl<<"nCode:"<<nCode
    //     <<endl<<"wParam:"<<wParam
    //     <<endl<<"vkCode:"<<kbStruct->vkCode
    //     <<endl<<"scanCode:"<<kbStruct->scanCode
    //     <<endl<<"flags:"<<kbStruct->flags
    //     <<endl<<"time:"<<kbStruct->time
    //     <<endl<<"dwExtraInfo:"<<kbStruct->dwExtraInfo
    // ;
    // qDebug()<<getpid();
    if (nCode <0)return CallNextHookEx(NULL, nCode, wParam, lParam);
    // WORD vkCode = LOWORD(wParam);                                 // virtual-key code
    // WORD keyFlags = HIWORD(lParam);
    // WORD scanCode = LOBYTE(keyFlags);                             // scan code
    // BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix
    // if (isExtendedKey)
    //     scanCode = MAKEWORD(scanCode, 0xE0);
    // BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
    // if(wasKeyDown){
    //     qDebug()<<"wasKeyDown";
    //     return CallNextHookEx(NULL, nCode, wParam, lParam);
    // }
    // WORD repeatCount = LOWORD(lParam);                            // repeat count, > 0 if several keydown messages was combined into one message
    // BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;  
    qint64  currentTime = QDateTime::currentMSecsSinceEpoch();
    QString info = "";// 虚拟按键的打印字符
    QString type = "";
    QString action = ' ';//vk_key
    
    switch(kbStruct->vkCode)
    {
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT: 
            info = "Shift";
            break;
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_CONTROL:
            info = "Control";
            break;
        case VK_LMENU:
        case VK_RMENU:
        case VK_MENU:
            info = "Alt";
            break;
        case VK_TAB:
            info = "Tab";
            break;
        case VK_CAPITAL:
            info = "Caps Lock";
            break;
        case VK_ESCAPE:
            info = "Esc";
            break;
        case VK_SPACE:
            info = "Space";
            break;
        case VK_RETURN:
            info = "Enter";
            break;
        case VK_BACK:
            info = "Backspace";
            break;
        case VK_LEFT:
            info = "Left";
            break;
        case VK_DELETE:
            info = "Delete";
            break;
        default:
            info = QString(static_cast<char>(kbStruct->vkCode));
            break;
    }
    switch(wParam)
    {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: //按下alt键后的消息
            type = "DOWN";
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            type = "UP";
            break;
    }
    action = QString::number(kbStruct->vkCode);
    if (keyboardThread)
    {
        // QString message = QString::number(currentTime)+":"+type+":"+info;
        // emit keyboardThread->sendNewAction(message);
        emit keyboardThread->sendNewRecord(QString::number(currentTime)+":"+type+":"+action);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI KeyboardMessageLoop(LPVOID lpParam)
{
    emit keyboardThread->sendNewAction(QString("[in] KeyboardMessageLoop()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_QUIT){
            qDebug()<<"recordingThread::run() WM_QUIT";
            break;
        }
        // TranslateMessage(&msg);
        DispatchMessage(&msg);  //钩子函数的执行与消息循环在同一个线程中
    }
    UnhookWindowsHookEx(keyboardHook);
    emit keyboardThread->sendNewAction("[out] KeyboardMessageLoop()");
    return 0;
}

void recordingThread::run(){
    emit sendNewAction(QString("[in] recordingThread::run()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    
    hThread = CreateThread(NULL, 0, KeyboardMessageLoop, NULL, 0, NULL);
    emit sendNewAction("    [call] CreateThread KeyboardMessageLoop");
    // 等待线程完成
    WaitForSingleObject(hThread, INFINITE);
    // 关闭句柄
    CloseHandle(hThread);
    emit sendNewAction("[out] recordingThread::run()");
    
}

