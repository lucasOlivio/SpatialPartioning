local Command = require("assets.scripts.commands.command")

-- Move the entity to the referenced location in "time"
-- easyIn and easyOut are optional, to set the proportion of the time that the obj will be accelerating/decelerating
-- stopAtEnd also optional, should the object stop velocity and accelerating at end or keep going?
function MoveCommand(entity, location, time, easyIn, easyOut, stopAtEnd)
    easyIn    = easyIn or 0.2
    easyOut   = easyOut or 0.1
    stopAtEnd =  (stopAtEnd ~= false) -- workaround to have stopatend default as true
    return Command:new("MoveTo", {entity = entity, location = location, 
                                  time = time, easyIn = easyIn, easyOut = easyOut,
                                  stopAtEnd = stopAtEnd})
end

-- Rotate the entity to the referenced orientation in "time"
-- easyIn and easyOut are optional, to set the proportion of the time that the obj will be accelerating/decelerating
-- stopAtEnd also optional, should the object stop velocity and accelerating at end or keep going?
function OrientCommand(entity, time, orientation, position, easyIn, easyOut, stopAtEnd)
    easyIn    = easyIn or 0.2
    easyOut   = easyOut or 0.1
    stopAtEnd =  (stopAtEnd ~= false) -- workaround to have stopatend default as true

    local tableOrient = {entity = entity, time = time, easyIn = easyIn, easyOut = easyOut,
                        stopAtEnd = stopAtEnd}

    if orientation then 
        tableOrient["orientation"] = orientation
    else
        tableOrient["position"] = position
    end

    return Command:new("OrientTo", tableOrient)
end

-- Move the object trough a bezier curve defined by the list of control points 
-- controlPoints, a list (size 3) of vec3 to define the bezier 
-- (The current position will be added to the first index by the engine)
-- timeStep, # of steps to calculate the curve
-- accRatio and deaccRatio are optional, to set the proportion of the time that the obj will be accelerating/decelerating
-- stopAtEnd also optional, should the object stop velocity and accelerating at end or keep going?
function FollowCurveCommand(entity, controlPoints, time, timeStep, accRatio, deaccRatio, stopAtEnd)
    timeStep   = timeStep or 10
    accRatio   = accRatio or 0.2
    deaccRatio = deaccRatio or 0.1
    stopAtEnd  =  (stopAtEnd ~= false) -- workaround to have stopatend default as true
    return Command:new("FollowCurve", {entity = entity, controlPoints = controlPoints, 
                                    time = time, timeStep = timeStep, 
                                    accRatio = accRatio, deaccRatio = deaccRatio,
                                    stopAtEnd = stopAtEnd})
end

-- Follow a target updating the targets position every "timeStep" seconds
-- targetTag, name of target to follow
-- timeStep, Seconds until updating target position
-- maxSpeed, maximum speed to reach
-- followDistance optional, max distance to reach from object
-- offset optional, Offset from target position
-- followForever optional, whether it should run forever or just get to target location
-- easyIn and easyOut optional, set distance from target to accelerate/decelerate (in %)
function FollowObjectCommand(entity, targetTag, timeStep, maxSpeed, maxRotationSpeed, followDistance, offset, followForever, easyIn, easyOut)
    followDistance = followDistance or 0.0
    offset = offset or {0.0, 0.0, 0.0}
    followForever  =  (followForever ~= false) -- workaround to have default as true
    easyIn  = easyIn or 0.2
    easyOut = easyOut or 0.1
    return Command:new("FollowObject", {entity = entity, targetTag = targetTag, 
                                    timeStep = timeStep, maxSpeed = maxSpeed, maxRotationSpeed = maxRotationSpeed,
                                    followDistance = followDistance, offset = offset,
                                    easyIn = easyIn, easyOut = easyOut,
                                    followForever = followForever})
end

-- Destroy the given entity and replaces with another
-- entity, entity id to destroy 
function DestroyEntityCommand(entity)
    return Command:new("DestroyEntity", {entity = entity})
end

-- Create an entity copied from the "entity" parameter
-- entity, entity id to create copy from
function CreateEntityCommand(entity, position, orientation)
    return Command:new("CreateEntity", {entity = entity, position = position, orientation = orientation})
end

return {
    MoveCommand = MoveCommand, 
    OrientCommand = OrientCommand, 
    FollowCurveCommand = FollowCurveCommand,
    FollowObjectCommand = FollowObjectCommand,
    DestroyEntityCommand = DestroyEntityCommand,
    CreateEntityCommand = CreateEntityCommand
}