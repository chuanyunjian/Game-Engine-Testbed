/*
-----------------------------------------------------------------------------

Copyright (c) 2010 Nigel Atkinson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include <engine.h>
#include <inputeventdata.h>
#include <windoweventdata.h>
#include <OgreLogManager.h>
#include <luaresource.h>

//For testing
#include <navigationmesh.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <time.h>
void msleep(int ms)
{
    struct timespec time;
    time.tv_sec = ms / 1000;
    time.tv_nsec = (ms % 1000) * (1000 * 1000);
    while( nanosleep(&time, &time) == -1 && errno == EINTR )
        continue; // We got woken by a system signal, go back to sleep
}
#else
#error "Figure out what header for some sleep func, on your platform"
#endif

void bindLuaConsole( lua_State *L );    // From luaconsolebinding.cpp

Engine::~Engine()
{
    // Order of shutdown matters.
    console             .shutdown();
    gameEntityManager   .shutdown();
    scriptingSystem     .shutdown();
    inputSystem         .shutdown();
    renderSystem        .shutdown();
}

bool Engine::initialise()
{
    // Managers may enque events as the start
    eventManager.addListener( this );
    eventManager.addListener( &renderSystem );
    eventManager.addListener( &scriptingSystem );
    eventManager.addListener( &inputSystem );

    if( ! renderSystem.initialise() )
        return false;

    inputSystem.initialise( renderSystem.getWindow(), false );
    scriptingSystem.initialise();

    // TODO: This could be better done by making the renderSystem a listener,
    // and send frame events.
    renderSystem.addFrameListener( &scriptingSystem );

    console.init( renderSystem.getRoot(), scriptingSystem.getInterpreter() );
    bindLuaConsole( scriptingSystem.getInterpreter() );

    gameEntityManager.initialise();

    Ogre::WindowEventUtilities::addWindowEventListener(renderSystem.getWindow(), this);

    stop = false;

    return true;
}

void Engine::run()
{
    while( stop == false )
    {
        Ogre::WindowEventUtilities::messagePump();
        inputSystem.capture();
        eventManager.processEvents();
        gameEntityManager.update();
        renderSystem.renderOneFrame();

        // Play nice with the operating system by sleeping a little.
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        msleep( 10 );
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        sleep( ??? );   // Might be the same as Linux.
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        Sleep( 10 );    // TODO: Check this func takes milliseconds!
#endif

    }
}

bool Engine::EventNotification( EventPtr event )
{
    if( event->type == Event::hash( "MSG_QUIT" ) )
    {
        stop = true;
        return true;
    }

    if( event->type == Event::hash( "EVT_KEYDOWN" ) )
    {
        boost::shared_ptr<InputEventData> data = boost::dynamic_pointer_cast<InputEventData>( event->data );

        if( data->key == OIS::KC_F12 )   // Emergency stop.
        {
            stop = true;
            return true;
        }
        if( data->key == OIS::KC_GRAVE )
        {
            console.setVisible( ! console.isVisible() );
            return true;
        }
        if( console.isVisible() )
        {
            OIS::KeyEvent arg( 0, data->key, data->parm );
            console.injectKeyPress( arg );
            return true;
        }
        if( data->key == OIS::KC_T )
        {
            // Testing.
            Ogre::Root *root = Ogre::Root::getSingletonPtr();
            Ogre::SceneManager* mgr = root->getSceneManager( "SceneManagerInstance" );
            Ogre::Entity *eTest = mgr->createEntity( "navtest.nav" );

            NavigationMesh navMesh;

            navMesh.BuildFromOgreMesh( eTest->getMesh() );

            mgr->destroyEntity( eTest );

            // Test out pathfinding.
            NavigationPath* path = navMesh.findNavigationPath( Ogre::Vector3( 8, 0, 8 ), Ogre::Vector3( -8, 0, -8 ) );

            if( path )
            {
                for( NavigationPath::iterator i = path->begin(); i != path->end(); i++ )
                    std::cout << "NavPoint: " << *i << std::endl;
            }

            navMesh.DebugTextDump( std::cout );

            return true;
        }
    }

    return false;
}

//Adjust mouse clipping area
void Engine::windowResized( Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int top, left;

    EventPtr event( new Event( "EVT_WINDOW_RESIZE" ) );
    boost::shared_ptr<WindowEventData> data( new WindowEventData );

    rw->getMetrics(width, height, depth, left, top);

    event->data = data;
    data->height = height;
    data->width = width;

    queueEvent( event );
}

//Unattach OIS before window shutdown (very important under Linux)
void Engine::windowClosed( Ogre::RenderWindow* rw)
{
    queueEvent( EventPtr( new Event( "MSG_QUIT" ) ) );
}
