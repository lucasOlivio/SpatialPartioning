local commands = require("assets.scripts.commands")

function Follow(entity, targetTag, timeStep, maxSpeed, maxRotationSpeed, followDistance, offset)
    x, y, z, rx, ry, rz, scale = GetTransform(targetTag)

    local followobjCommand = commands.FollowObjectCommand(entity, targetTag, timeStep, maxSpeed,
                                                          maxRotationSpeed, followDistance, offset)

    local mainCommandGroup = CommandGroup:new("followGroup")
    mainCommandGroup:addCommand(followobjCommand, true) 

    commands.DispatchCommands(mainCommandGroup)
end

return {
    Follow = Follow
}