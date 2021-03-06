local scene = Ogre.getSceneManager()
am = AnimationManager.getSingleton()

function moveTo( v )
    local ma = MovementAnimation( player.node, v, player.walkspeed )
    am:add(ma)
    ma:start()
    return ma
end

function rotateTo( heading )
    local src = player.node:getOrientation() * Vector3.UNIT_X
    -- Prevent toppling over by restricting rotation to Y
    heading.y = src.y
    local quat = src:getRotationTo( heading, Vector3.UNIT_Y )
    local ra = RotationAnimation( player.node, quat, 5 )
    am:add(ra)
    ra:start()
    return ra
end

function getDirectionTo( v )
    local d = v - player.node:getPosition()
    d:normalise()
    return d
end

function followList( list )
    if type(list) ~= 'table' then return end
    local wa = MeshAnimation( player.mesh, 'Walk' )
    am:add(wa)
    wa:setWeight(0)
    wa:setFadeSpeed(2)
    wa:start()
    wa:fadeIn()
    player.walking = true
    player.stopwalking = false
    player.walkspeed = player.walkspeed or 30
    for i, vector in pairs(list) do
        if i ~= 1 then
            local r
            if r ~= nil and r:isFinished() == false then
                r:stop();
                print 'Stopping previous turn'
            end
            local r = rotateTo( getDirectionTo( vector ) )
            local m = moveTo( vector )
            while not m:isFinished() and not player.stopwalking do
                yield()
            end
            if player.stopwalking == true then
                m:stop()
                am:remove(m)
                break
            end
        end
    end
    player.walking = false
    wa:fadeOut()
    while wa:isFadingOut() do
        yield()
    end
    am:remove(wa)
end

function getpath()
    local p = player.node:getPosition()
    local d = getWalkableEntityHitPosition(mouse.x/mouse.width, mouse.y/mouse.height)
    local maxAngle = Radian( Degree(90) )

    return nv:findPath( p, d, maxAngle, 10 )
end

function walkTask()
    player.stopwalking = true
    while player.walking == true do
        yield()
    end
    startwalk = false
    followList(path)
end

function walk()
    if startwalk then return end
    startwalk = true
    path = getpath()
    createTask( walkTask )
end

bind( KeyCodes.KC_G, walk )
