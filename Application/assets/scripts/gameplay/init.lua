local destroyship = require("assets.scripts.gameplay.destroyship")
local shoot = require("assets.scripts.gameplay.shoot")
local flee = require("assets.scripts.gameplay.flee")
local follow = require("assets.scripts.gameplay.follow")

return 
{
    DestroyShip = destroyship.DestroyShip,
    Shoot = shoot.Shoot,
    Flee = flee.Flee,
    Follow = follow.Follow
}