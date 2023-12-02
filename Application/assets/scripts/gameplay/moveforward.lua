local commands = require("assets.scripts.commands")

-- Move forward in the direction the object is facing 
-- (considering +z facing forward without rotation)
function MoveForward(entity, speed)
    local mainCommandGroup = CommandGroup:new("moveforward")
    x, y, z, rx, ry, rz, scale = GetTransform(entity)
    dirX, diry, dirz = GetFrontDirection(entity)

    local moveto = {
        x + dirX,
        y + dirY,
        z + dirZ
    }
    local moveCommand = commands.MoveCommand(entity, moveto, 6, 0.0, 0.0, false)

    mainCommandGroup:addCommand(moveCommand, false) 

    commands.DispatchCommands(mainCommandGroup)
end

return {
    Flee = Flee
}
