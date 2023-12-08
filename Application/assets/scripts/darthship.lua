local accelerate   = require("assets.scripts.gameplay.accelerate")
local followmouse   = require("assets.scripts.gameplay.followmouse")
local directions   = require("assets.scripts.common.directions")
local inputkeys    = require("assets.scripts.common.inputkeys")
local inputactions = require("assets.scripts.common.inputactions")

tbGlobals = {
    maxSpeed     = 7.5,
    acceleration = 2,
    sensitivity  = 0.007,
    accUUID      = 0
}

function StartAcceleration(direction)
    success, accUUID = accelerate.Accelerate(entity, 
                                             direction, 
                                             tbGlobals["acceleration"], 
                                             tbGlobals["maxSpeed"])

    -- Save accelerate ID state to stop later
    tbGlobals["accUUID"] = accUUID
end

function onstart()
    print("startn entity: " .. entity)

    followmouseUUID = followmouse.FollowMouse(entity, tbGlobals["sensitivity"])
    tbGlobals["followmouseUUID"] = followmouseUUID
end

function onkeyinput(pressedkey, action, mods, scancode)

    if (pressedkey == inputkeys.W and (action == inputactions.PRESS)) then -- W Pressed
        StartAcceleration(directions.FORWARD)
    elseif (pressedkey == inputkeys.S and action == inputactions.PRESS) then -- S Pressed
        StartAcceleration(directions.BACKWARD)
    end
        
    if (action == inputactions.RELEASE) then
        if(tbGlobals["accUUID"] > 0) then  -- W or S Released
            CancelCommand(tbGlobals["accUUID"])
        end
    end
end
