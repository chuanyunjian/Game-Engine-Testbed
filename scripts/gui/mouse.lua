function setupMouse()
    local mgr=getOverlayManager()
    mouseGui=mgr:createOverlay('MouseOverlay')
    mousePointer=mgr:createElement('Panel','MouseCursor',false)
    mousePointer:setParameter('material','mouse/arrow')
    mousePointer:setParameter('metrics_mode','pixels')
    mousePointer:setParameter('width','64')
    mousePointer:setParameter('height','48')
    mouseGui:add2D(mousePointer)
    mouseGui:setZOrder(650)
    --mouseGui:show()
end

mouse={}
function mouse.show()
    mouse.i = (mouse.i or 0 ) + 1

    if mouse.i > 0 then
        mouseGui:show()
    end
end

function mouse.hide()
    mouse.i = (mouse.i or 1) - 1
    if mouse.i < 0 then 
        mouse.i = 0
    end
    if mouse.i == 0 then 
        mouseGui:hide()
    end
end
    

function setMouseCursorPosition( x, y )
    mousePointer:setPosition( x-32, y-24 )
end

createTask( setupMouse )