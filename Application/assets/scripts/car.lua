local accelerate = require("assets.scripts.gameplay.accelerate")
local inputkeys   = require("assets.scripts.common.inputkeys")
local inputactions   = require("assets.scripts.common.inputactions")

tbGlobals = {
    maxSpeed = 3000,
    acceleration = 750
}

function onstart()
    print("startn entity: " .. entity)
    print("majxspeed: " .. tbGlobals.maxSpeed)
    tbGlobals["maxSpeed"] = tbGlobals["maxSpeed"] + 1000
end

function onkeyinput(pressedkey, action, mods, scancode)
    print("inputing entity: " .. entity)
    print("majxspeed: " .. tbGlobals.maxSpeed)

    if (pressedkey == inputkeys.W and inputactions.PRESS == action ) then      -- W Pressed
        accUUID = accelerate.Accelerate(entity, common.directions.FORWARD, acceleration, maxSpeed)

        -- Save accelerate ID state to later
        tbGlobals["accUUID"] = accUUID
    elseif (pressedkey == inputkeys.W and inputactions.RELEASE == action ) then -- W Released
        CancelCommand(tbGlobals["accUUID"])
    end
end
