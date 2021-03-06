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
#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include <list>
#include <string>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSingleton.h>
#include <OgreLogManager.h>
#include <boost/shared_ptr.hpp>
#include <boost/functional/hash.hpp>
#include <lua.hpp>

class GameEntity;
class GameEntityManager;

typedef boost::shared_ptr<GameEntity> GameEntityPtr;

class GameEntity
{
    friend class GameEntityManager;

    // Name
    std::string name;
    // HashID
    size_t hashId;
    // Original material
    Ogre::MaterialPtr originalMaterial;
    // Material used for highlight.
    Ogre::MaterialPtr highlightMaterial;

public: // TODO: Make private and add accessor funcs.

    GameEntityPtr parent;

    // Scene Node
    Ogre::SceneNode* sceneNode;
    // Entity
    Ogre::Entity* mesh;

    // Physics Shape
    // btSomethingErRather* physical;

public:
    GameEntity();
    virtual ~GameEntity();

    // Accessors
    std::string getName() const { return name; }
    void setName( std::string newName );

    size_t getHashId() { return hashId; }

    virtual void update();

    // If this instance is being managed, it is removed.
    void removeFromManager();

    // Checks if a ray from camera at (x,y) intersects with *any* mesh triangles.
    // Returns as soon as an intersection is found.
    bool hitCheck( float x, float y );
    // Returns the position of intersection with the mesh triangle that results in
    // the smallest distance from the camera.
    Ogre::Vector3 hitPosition( float x, float y );

    static boost::hash<std::string> hasher;
    size_t hash( std::string str )
    {
        return hasher(str);
    }
    bool operator==( const GameEntityPtr rhs )
    {
        return hashId == rhs->hashId;
    }

    void highlight( bool on = true );
    void createHighlightMaterial();

    bool isVisible();
    void setVisible( bool visible );
};

// This allows GameEntities passed from Lua to C++ and back
// to retain thier Lua parts.  In other words, not get sliced.
// Also Lua can derive from GameEntity and override the update
// method.
/*
class GameEntityWrapper : public GameEntity, public luabind::wrap_base
{
    public:

    virtual void update()
    {
        lua_State* L = m_self.state();
        m_self.get(L);
        if( ! lua_isnil( L, -1 ) ) // If the Lua side is not there anymore just ignore.
            call<void>( "update" );
        else
            Ogre::LogManager::getSingleton().stream() << "Game Entity Lua side missing.";
        lua_pop( L, 1 );
    }

    static void default_update( GameEntity* ptr )
    {
        ptr->GameEntity::update();
    }
};
*/

class GameEntityManager : public Ogre::Singleton<GameEntityManager>
{
    typedef std::pair<size_t, GameEntityPtr> mapping;

    std::map<size_t, GameEntityPtr> entities;

    Ogre::SceneManager* sceneManager;
    Ogre::RaySceneQuery* sceneQuery;

public:
    static GameEntityManager* getSingletonPtr();
    static GameEntityManager& getSingleton();

    GameEntityManager();
    ~GameEntityManager();

    void initialise();
    void shutdown();

    bool addGameEntity( GameEntityPtr p );
    void removeGameEntity( size_t hashId );
    void removeGameEntity( std::string name );
    GameEntityPtr getGameEntity( size_t hashId );
    GameEntityPtr getGameEntity( std::string name );

    void update();

    Ogre::Ray getCameraRay( float x, float y );
    std::list<GameEntityPtr> mousePick( float x, float y );
    void mousePickLua( lua_State* L, float x, float y );
    void getGameEntityList( lua_State *L );
};

void bindGameEntityClasses( lua_State* L );

#endif // GAMEENTITY_H
