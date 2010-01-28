#ifndef RENDERSYSTEM_H_INCLUDED
#define RENDERSYSTEM_H_INCLUDED

#include <Ogre.h>

class RenderSystem
{
    Ogre::Root* mRoot;
    Ogre::SceneManager* mSceneManager;
    Ogre::RenderWindow* mWindow;
    Ogre::Camera* mCamera;

public:
    RenderSystem() : mRoot(0), mSceneManager(0), mWindow(0), mCamera(0)
    {
    }
    ~RenderSystem()
    {
        shutdown();
    }

    void shutdown();
    bool initialise();

    Ogre::RenderWindow* getWindow() { return mWindow; }
    Ogre::Root* getRoot() { return mRoot; }
    void addFrameListener( Ogre::FrameListener *listener ) { mRoot->addFrameListener( listener ); }

    void renderOneFrame();
};

#endif // RENDERSYSTEM_H_INCLUDED