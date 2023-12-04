local accelerate = require("assets.scripts.gameplay.accelerate")
local common   = require("assets.scripts.common")

local entity       = "camera"
local maxSpeed     = 1500
local acceleration = 200

function onstart()
    print("startn entity: " .. entity)
    
    -- Example usage:
end

function onkeyinput(pressedkey, action, mods, scancode)
    if (pressedkey == common.inputkeys.W) then
        accelerate.Accelerate(entity, common.directions.FORWARD, acceleration, maxSpeed)
    end
end
