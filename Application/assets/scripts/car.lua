local accelerate = require("assets.scripts.gameplay.accelerate")
local common   = require("assets.scripts.common")

local entity       = "camera"
local maxSpeed     = 1500
local acceleration = 200

function onstart()
    print("startn entity: " .. entity)
    
    -- Example usage:
    accelerate.Accelerate(entity, common.directions.FORWARD, acceleration, maxSpeed)
end

function update(deltatime)
end
