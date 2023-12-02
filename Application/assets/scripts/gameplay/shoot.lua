local commands = require("assets.scripts.commands")

function Shoot(entity)
    x, y, z, rx, ry, rz, scale = GetTransform(entity)

    local selfdestroy = commands.DestroyEntityCommand(entity)
    local destroiedship = commands.CreateEntityCommand("missle", {x, y, z}, {rx, ry, rz})

    local mainCommandGroup = CommandGroup:new("shoot")
    mainCommandGroup:addCommand(selfdestroy, true) 
    mainCommandGroup:addCommand(destroiedship, true) 

    commands.DispatchCommands(mainCommandGroup)
end

return {
    Shoot = Shoot
}
