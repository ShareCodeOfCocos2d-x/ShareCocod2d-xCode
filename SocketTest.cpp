//
//  SocketTest.cpp
//  AiliBigSword
//
//  Created by Haifeng on 13-7-8.
//
//

#include "SocketTest.h"
#include "cocos2d.h"
#include "GameCanvas.h"
#include "LMain.h"
#include "LoginLayer.h"

using namespace cocos2d;
using namespace std;

vector<PlayerDataStruct *> * SocketTest::playerJsonString = NULL;
vector<PlayerDataStruct *> * SocketTest::othersPlayer = NULL;

int SocketTest::mePID = 0;
int SocketTest::playerCount = 0;

SocketTest::SocketTest(){
    CCLog("SocketTest");
}
SocketTest::~SocketTest(){
}

void SocketTest::onEnter()
{
    CCLog("onEnter");
    CCLayer::onEnter();
    setTouchEnabled(true);
}

void SocketTest::onExit()
{
    CCLayer::onExit();
    this->setTouchEnabled(false);
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
}

bool SocketTest::init()
{
    CCLog("init");
    if(!JMBackgroundLayer::init()){
        return false;
    }
    test = new PlayerDataStruct();
    othersPlayer = new vector<PlayerDataStruct *>();
    playerJsonString = new vector<PlayerDataStruct *>();
    CCLog("SocketTest !");
    
    
    test = LMain::getInstance()->dataManager->showMyPlayer();
    
    test->p_xPosition = 480.0f;
    test->p_yPosition = 360.0f;
    test->playerInstance = CCSprite::create(test->p_image.c_str());
    test->playerInstance->setScale(0.5f);
    test->playerInstance->setPosition(ccp(480,360));
    this->addChild(test->playerInstance);
    
    
    
    CCMenuItem *back = CCMenuItemFont::create("返回", this, menu_selector(SocketTest::_bacKGameCanvas));
    CCMenu *menu = CCMenu::create(back,NULL);
    menu->setColor(ccYELLOW);
    menu->setPosition(ccp(30,130));
    this->addChild(menu);
    LMain::getInstance()->dataManager->startSever(test);
    schedule(schedule_selector(SocketTest::update),0.1f);
    CCLog("init over~");
    CCLog("init othersPlayer count is %d",othersPlayer->size());
    
    action = cocos2d::CCAction::create();
    return true;
}
void SocketTest::_bacKGameCanvas(CCObject *pSender)
{
    unscheduleAllSelectors();
    CCLog("remove before this->getChildrenCount() = %d",this->getChildrenCount());
    LMain::getInstance()->dataManager->closeSever(test,this);
    
    mePID = 0;
    playerCount = 0;
    
    othersPlayer->clear();
    othersPlayer = NULL;
    playerJsonString->clear();
    playerJsonString = NULL;
    
    CCScene *pGameCanvas = CCScene::create();
    GameCanvas *scene = GameCanvas::create();
    pGameCanvas->addChild(scene,0);
    CCDirector::sharedDirector()->replaceScene(pGameCanvas);
    CCLog("remove after this->getChildrenCount() = %d",this->getChildrenCount());
}

void SocketTest::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    this->stopAction(action);
    CCSetIterator it = pTouches->begin();
    CCTouch* touch = (CCTouch*)(*it);
    
    CCPoint loction = touch->getLocation();
    float speed = sqrtf((loction.x-test->playerInstance->getPositionX())*(loction.x-test->playerInstance->getPositionX()) +
                        (loction.y-test->playerInstance->getPositionY())*(loction.y-test->playerInstance->getPositionY()));
    
    action = CCMoveTo::create(speed / 300.0f, loction);
    test->playerInstance->runAction(action);
}

int SocketTest::start()
{
    int ret;
    ret = pthread_create(&pid, NULL, othersPthread, NULL);
    if(ret!=0){
        CCLog("pthread error");
    }
    return 0;
}
void* SocketTest::othersPthread(void *para)
{
    SocketTest *temp = (SocketTest *) para;
    temp->doThreadCallBack();
    return NULL;
}

void SocketTest::doThreadCallBack()
{
    playerJsonString = LMain::getInstance()->dataManager->receiveInformation();
    othersPlayer = LMain::getInstance()->dataManager->playerStruct;
    for (int i = 0; i < playerJsonString->size(); i++) {
        if (othersPlayer->at(i)->p_uid == mePID)
            continue;
        CCLog("playerJsonString size = %d",playerJsonString->size());
        othersPlayer->at(i)->playerInstance->setPosition(ccp(playerJsonString->at(i)->p_xPosition,playerJsonString->at(i)->p_yPosition));
        CCLog("Position x = %f,Position y = %f",playerJsonString->at(i)->p_xPosition,playerJsonString->at(i)->p_yPosition);
        CCLog("playerJsonString (%d)",playerJsonString->size());	
        for (int i = 0; i < playerJsonString->size(); i++) {
        CCLog("playerJsonString at(%d) = %f",i,playerJsonString->at(i)->p_xPosition);
        }
    }
}

void SocketTest::update(float dt)
{
//    CCLog("LMain::getInstance()->dataManager->showOthersPlayer() size = %d",LMain::getInstance()->dataManager->showOthersPlayer()->size());
    CCLog("playerCount = %d",playerCount);
//    othersPlayer = LMain::getInstance()->dataManager->showOthersPlayer();
    if (playerCount != othersPlayer->size()) {
        CCLog("playerCount othersPlayer->size() = %d",othersPlayer->size());
        if (othersPlayer->size() >= 2) {
            CCLog("init update othersPlayer->size = %d",othersPlayer->at(0)->p_uid);
            for (int i = 0; i < othersPlayer->size(); i++) {
                if (othersPlayer->at(i)->p_uid == test->p_uid) {
                    continue;
                }
                CCLog("update othersPlayer->at(i)->pHeadPic.c_str() %s",LMain::getInstance()->dataManager->playerStruct->at(i)->p_image.c_str());
                CCLog("init update othersPlayer->size = %d",othersPlayer->size());
                int tag = othersPlayer->at(i)->p_uid;
                CCLog("tag = %d",tag);
                othersPlayer->at(i)->playerInstance = CCSprite::create(othersPlayer->at(i)->p_image.c_str());
                othersPlayer->at(i)->playerInstance->setScale(0.5f);
                othersPlayer->at(i)->playerInstance->setPosition(ccp(othersPlayer->at(i)->p_xPosition,othersPlayer->at(i)->p_yPosition));
                
                this->addChild(othersPlayer->at(i)->playerInstance);
                othersPlayer->at(i)->playerInstance->setTag(tag);
            }
        }
        playerCount = othersPlayer->size();
    }
//    LMain::getInstance()->dataManager->sendInformation(test);
    CCLog("remove before this->getChildrenCount() = %d",this->getChildrenCount());
    start();
}
