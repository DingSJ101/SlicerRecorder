#include "recordingthread.h"
#include <QDebug>
#include <QTime>
#include <vtkMRMLApplicationLogic.h>
recordingThread::recordingThread()
{
    qDebug() << "recordingThread::recordingThread()"<< this->thread()->currentThreadId();
    qDebug() << "recordingThread::recordingThread()"<< getpid();
}
recordingThread::recordingThread(int type):type(type)
{
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
    qint64  currentTime = QDateTime::currentMSecsSinceEpoch();
    auto mouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
    if(nCode <0) return CallNextHookEx(NULL, nCode, wParam, lParam);
    if ( wParam == WM_MOUSEMOVE)
    {
        // emit mouseThread->sendNewRecord(QString::number(currentTime)+QString::number(lParam),1);
        // int xPos = GET_X_LPARAM(lParam);
        // int yPos = GET_Y_LPARAM(lParam);
        int xPos = mouseStruct->pt.x;
        int yPos = mouseStruct->pt.y;
        emit mouseThread->sendNewRecord(QString::number(currentTime)+":"+QString::number(wParam)+":"+QString::number(xPos)+QString(":")+QString::number(yPos),1);
        // POINT cursorPos;
        // GetCursorPos(&cursorPos);
        // emit mouseThread->sendNewRecord(QString::number(currentTime)+":"+QString::number(cursorPos.x)+QString(":")+QString::number(cursorPos.y),1);
    }
    else {
        QString action="";
        QString param1="";
        switch(wParam){
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
                action = QString::number(WM_LBUTTONDOWN);
                break;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONDBLCLK:
                action = QString::number(WM_RBUTTONDOWN);
                break;
            case WM_MBUTTONDOWN:
            case WM_MBUTTONDBLCLK:
                action = QString::number(WM_MBUTTONDOWN);
                break;
            case WM_XBUTTONDOWN:
            case WM_XBUTTONDBLCLK://鼠标侧键
                break;

            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
                action = QString::number(wParam);
                break;
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
                action = QString::number(WM_MOUSEWHEEL);
                param1 = QString::number(GET_WHEEL_DELTA_WPARAM(mouseStruct->mouseData));
                break;
        }
        if (mouseThread)
        {
            emit mouseThread->sendNewRecord(QString::number(currentTime)+":"+action+":"+param1+":0",1);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI MouseMessageLoop(LPVOID lpParam)
{
    emit mouseThread->sendNewAction(QString("[in] MouseMessageLoop()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    HHOOK MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // if (msg.message == WM_QUIT){
        //     emit mouseThread->sendNewAction("recordingThread::run() WM_QUIT");
        //     break;
        // }
        // TranslateMessage(&msg);
        // DispatchMessage(&msg);  //钩子函数的执行与消息循环在同一个线程中
    }
    UnhookWindowsHookEx(MouseHook);
    emit mouseThread->sendNewAction("[out] MouseMessageLoop()");
    return 0;
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
        case VK_F11:
            info = "F11";
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
        emit keyboardThread->sendNewRecord(QString::number(currentTime)+":"+type+":"+action,0);
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
        // if (msg.message == WM_QUIT){
        //     emit keyboardThread->sendNewAction("recordingThread::run() WM_QUIT");
        //     break;
        // }
        // TranslateMessage(&msg);
        // DispatchMessage(&msg);  //钩子函数的执行与消息循环在同一个线程中
    }
    UnhookWindowsHookEx(keyboardHook);
    emit keyboardThread->sendNewAction("[out] KeyboardMessageLoop()");
    return 0;
}

void recordingThread::run(){
    emit sendNewAction(QString("[in] recordingThread::run()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    if (type==0){
        hThread = CreateThread(NULL, 0, KeyboardMessageLoop, NULL, 0, NULL);
        emit sendNewAction("    [call] CreateThread KeyboardMessageLoop");
    }
    else{
        hThread = CreateThread(NULL, 0, MouseMessageLoop, NULL, 0, NULL);
        emit sendNewAction("    [call] CreateThread MouseMessageLoop");
    }
    
    // 等待线程完成
    WaitForSingleObject(hThread, INFINITE);
    // 关闭句柄
    CloseHandle(hThread);
    emit sendNewAction("[out] recordingThread::run()");
    
}






void CaptureScreenAndSave(const char* filename) {
    // 获取屏幕尺寸
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // 创建设备上下文
    HDC hdcScreen = GetDC(NULL);
    screenWidth = GetDeviceCaps(hdcScreen, DESKTOPHORZRES);
    screenHeight = GetDeviceCaps(hdcScreen, DESKTOPVERTRES);
    
    std::cout<<screenWidth<<" "<<screenHeight<<std::endl;
    // 创建兼容位图
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);

    // 创建兼容设备上下文
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    // 将兼容位图与兼容设备上下文关联
    SelectObject(hdcMem, hBitmap);

    // 使用 BitBlt 函数进行截图
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    // 保存截图为文件（这里保存为 BMP 格式）
    SaveBMPFile(filename, hBitmap, hdcMem, screenWidth, screenHeight);
    // 释放资源
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    DeleteObject(hBitmap);
}
void SaveBMPFile(const char* filename, HBITMAP hBitmap, HDC hdcMem, int width, int height) {
    // 使用 GDI 函数将位图保存为 BMP 文件
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    // 设置文件头
    fileHeader.bfType = 0x4D42;  // "BM" 表示 BMP 文件
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bitmap.bmWidthBytes * bitmap.bmHeight;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // 设置位图信息头
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = bitmap.bmWidth;
    infoHeader.biHeight = -bitmap.bmHeight; // 为了将图像上下颠倒
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = bitmap.bmBitsPixel;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    // 打开文件并写入数据
    FILE* file = fopen(filename, "wb");
    if (file) {
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

        // 按行反转位图数据，然后写入文件
        int lineSize = ((bitmap.bmWidth * bitmap.bmBitsPixel + 31) / 32) * 4;
        char* lineBuffer = new char[lineSize];
        for (int y = bitmap.bmHeight - 1; y >= 0; --y) {
            int offset = y * lineSize;
            GetDIBits(hdcMem, hBitmap, y, 1, lineBuffer, (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);
            fwrite(lineBuffer, lineSize, 1, file);
        }
        delete[] lineBuffer;
        fclose(file);
    }
}

qint64 delta_time = -1;
void setDeltaTime(qint64 time)
{
    delta_time = time;
    qDebug()<<("set delta_time = "+QString::number(delta_time));
}
qint64 getDeltaTime()
{
    return delta_time;
}