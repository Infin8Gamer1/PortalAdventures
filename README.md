# PortalAdventures
A UE5 game with portal mechanics similar to Valve's Portal
![example of shooting portals](/Wiki/Media/PortalShootingExample.gif)

## Known Bugs
- Lumen currently does not work with Scene Capture so lighting in portals doesnt look corrrect. 
	* Check this [Pull Request](https://github.com/EpicGames/UnrealEngine/pull/9074) for potential fix.
- Current Implementation of portal collisions disables the entire objects collision that the portal is placed on, This can lead to unintended behavior if the player is standing on the object and is in the portal trigger
	* Workaround would be to use sepperate objects for the "floor" and "wall"
	* Could edit the collision mesh at runtime to "punch a hole" for the portal
- Weapon Clips with portal when crossing
	* Easy Fix would be to draw weapon and first person arms on top of everything however Unreal Engine doesn't support this
	* Could implement a fix in the weapon material that scales weapon twords camera [Tutorial](https://www.youtube.com/watch?v=zqfzvHCcvZs&ab_channel=JackFrench)
		- still clips if standing in the portal
- When carrying a cube through a portal the target holding position doesnt change until player is "teleported"
	* this causes held object to freak out
	* Fix for this would be to implement raycasts that "get teleported" if shot through a portal
		- would need to update how we pickup objects to use raycasts
- When passing an object through a portal it currently doesn't appear on the other side if it is less than 1/2 way through
	* Fix for this is to create a coppy of the mesh that is at the exit portal before the object gets teleported
		- Example in this [video at 6:20](https://youtu.be/_SmPR5mvH7w?t=380)

## Credits
- [Valve's Portal](https://store.steampowered.com/app/400/Portal/)
- [Creating Seamless Portals in UE4](https://www.froyok.fr/blog/2019-03-creating-seamless-portals-in-unreal-engine-4/)
- [zachdavids/PortalSystem](https://github.com/zachdavids/PortalSystem)
- [FreetimeStudio/PortalPlugin](https://github.com/FreetimeStudio/PortalPlugin)