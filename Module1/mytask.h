#ifndef MYTASK_H
#define MYTASK_H

#include <stdio.h>
#include <qreadwritelock.h>
#include <string>
#include <QThread>
#include <recordingthread.h>
#include <QDebug>
#include <QThreadPool>

class myTask    
:public QThread
{
    Q_OBJECT;

public:
    explicit myTask(QObject* parent = nullptr);
public:
    void reproduce();
    void simplify();
public slots:
    Q_INVOKABLE void initialize(); // 初始化文件指针
    // 获取QString内容，并写入文件
    // QString格式为：时间戳:事件:vk_key
    Q_INVOKABLE void addRecord(QString); 
    // 获取QString内容，并前端展示
    Q_INVOKABLE void addAction(QString); 
    Q_INVOKABLE void finish(); // 结束记录
    
Q_SIGNALS:
    void echoInfoImp(QString info); // 输出文件信息
    void stopThread();


// private:
    
public:
    FILE *fp=nullptr;
    const char* filename = "D:\\text.txt";
    const char* newfilename = "D:\\text_new.txt";
    QReadWriteLock lock;
    QThread* m_thread = nullptr;
    recordingThread* mouseTask = nullptr;
    recordingThread* keyboardTask = nullptr;
    
};

#endif // MYTASK_H
