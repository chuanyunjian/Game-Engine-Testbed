  quest={}
journal={}
    msg={}
QUEST_START=1
QUEST_TASK_1_FINISHED=2
QUEST_TASK_A_START=3
QUEST_TASK_A_FINISED=4
QUEST_TASK_B_START=5
QUEST_TASK_B_FINISHED=6
QUEST_TASK_LAST_START=7
QUEST_TASK_LAST_FINISHED=8

msg[QUEST_START]                ="Your quest has started!"
msg[QUEST_TASK_1_FINISHED]      ="You have made your choice, now do it!"
msg[QUEST_TASK_A_START]         ="Right kill them all!"
msg[QUEST_TASK_A_FINISHED]      ="Thier all dead as door nails."
msg[QUEST_TASK_B_START]         ="Ok, it's up to you to save them all!"
msg[QUEST_TASK_B_FINISHED]      ="Hooray you did it.  They are all safe now."
msg[QUEST_TASK_LAST_START]      ="Well done, now return home."
msg[QUEST_TASK_LAST_FINISHED]   ="Home at last, thats it for this quest!"

-- Public interface
function quest:init()
    print 'Init'
    self.taskinfo = {}
    self.taskinfo[self.task1] = {}
    self.taskinfo[self.taskA] = {}
    self.taskinfo[self.taskB] = {}
    self.taskinfo[self.taskLast] = {}
    self:task1_init()
end

function quest:update()
    self:currentTask()
end

function quest:status()
    return "active"
end

-- Protected members
function quest:task1_init()
    self.taskinfo[self.task1].finished = false
    self.currentTask = self.task1
    journal:add( msg[QUEST_START] )
end

function quest:task1()
    -- check if first task is complete.
    if choice == 1 then
        self.taskinfo[self.task1].finished = true
        journal:add( msg[QUEST_TASK_1_FINISHED] )
        self:taskA_init()
    end
    if choice == 2 then
        self.taskinfo[self.task1].finished = true
        journal:add( msg[QUEST_TASK_1_FINISHED] )
        self:taskB_init()
    end
end

function quest:taskA_init()
    self.taskinfo[self.taskA].finished = false
    self.currentTask = self.taskA
    journal:add( msg[QUEST_TASK_A_START] )
end

function quest:taskA()
    if test == 'kill' then
        self.taskinfo[self.taskA].finished = true
        journal:add( msg[QUEST_TASK_A_FINISHED] )
        self:taskLast_init()
    end
end

function quest:taskB_init()
    self.taskinfo[self.taskB].finished = false
    self.currentTask = self.taskB
    journal:add( msg[QUEST_TASK_B_START] )
end

function quest:taskB()
    if test == 'save' then
        self.taskinfo[self.taskB].finished = true
        journal:add( msg[QUEST_TASK_B_FINISHED] )
        self:taskLast_init()
    end
end

function quest:taskLast_init()
    self.taskinfo[self.taskLast].finished = false
    self.currentTask = self.taskLast
    journal:add( msg[QUEST_TASK_LAST_START] )
end

function quest:taskLast()
    if test == 'home' then
        self.taskinfo[self.taskLast].finished = true
        journal:add( msg[QUEST_TASK_LAST_FINISHED] )
        function self:status() return "finished" end
    end
end

-- Journal
function journal:init()
    self.entries = {}
end

function journal:add( message )
    local entry = { msg = message, time = os.clock() }
    table.insert( self.entries, entry )
    print( message )
end

function journal:view()
    print 'Journal Entries'
    for k,v in pairs( self.entries ) do
        print( v.msg, v.time )
    end
end

journal:init()
