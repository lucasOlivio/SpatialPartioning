local json = require("assets.scripts.json")

-- Converts command batch into a JSON and sends to engine scripting system
function DispatchCommands(commands, isforever)
    -- Convert the serialized result to a JSON string
    local jsonString = json.stringify(commands, false)
    -- Send to engine
    return SendCommands(jsonString, isforever)
end

return 
{
    DispatchCommands = DispatchCommands
}