//
//  SocketTest.h
//  AiliBigSword
//
//  Created by Haifeng on 13-7-8.
//
//

#ifndef __AiliBigSword__SocketTest__
#define __AiliBigSword__SocketTest__

#include <iostream>
#include "cocos2d.h"
#include "JMBackgroundLayer.h"
#include "LMain.h"
//#include "InitPlayer.h"

using namespace std;
using namespace cocos2d;

class SocketTest : public JMBackgroundLayer
{
private:
    CCAction *action;
    static int playerCount;
    static int mePID;
    pthread_t pid ;
    PlayerDataStruct *test; //本玩家
    static vector<PlayerDataStruct *> *othersPlayer;        //存储当前游戏中得其他玩家信息
    static vector<PlayerDataStruct *> *playerJsonString;    //存储当前所有玩家的Json解析后的数据
public:
    
    virtual bool init();
    
    SocketTest();
    ~SocketTest();
    virtual void onEnter();
    virtual void onExit();
    
    void _bacKGameCanvas(CCObject *pSender);
    void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
    
    int start();
    
    void doThreadCallBack();
    
    static void* othersPthread(void *para);
    void update(float dt);
    CREATE_FUNC(SocketTest);
};
#endif /* defined(__AiliBigSword__SocketTest__) */
