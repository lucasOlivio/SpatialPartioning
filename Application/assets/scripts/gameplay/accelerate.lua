local commands = require("assets.scripts.commands")

-- Accelerates the object to the given direction
function Accelerate(entity, direction, acceleration, maxSpeed)
    local mainCommandGroup = CommandGroup:new("accelerate")

    local moveCommand = commands.AccelerateTowards(entity, direction, acceleration, maxSpeed)

    mainCommandGroup:addCommand(moveCommand, false) 

    commands.DispatchCommands(mainCommandGroup)
end

return {
    Accelerate = Accelerate
}
