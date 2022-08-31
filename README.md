# PortalAdventures
A UE5 game with portal mechanics similar to Valve's Portal
![example of shooting portals](/Wiki/Media/PortalShootingExample.gif)

## Known Bugs
- Lumen currently does not work with Scene Capture so lighting in portals doesnt look corrrect. 
	* Check this [Pull Request](https://github.com/EpicGames/UnrealEngine/pull/9074) for potential fix.
- Weapon Clips with portal when crossing
	* Easy Fix would be to draw weapon and first person arms on top of everything however Unreal Engine doesn't support this
	* Could implement a fix in the weapon material that scales weapon twords camera [Tutorial](https://www.youtube.com/watch?v=zqfzvHCcvZs&ab_channel=JackFrench)
		- still clips if standing in the portal
- When carrying a cube through a portal the target holding position doesnt change until player is "teleported"
	* Causes held object to freak out

## Credits
- [Valve's Portal](https://store.steampowered.com/app/400/Portal/)
- [Creating Seamless Portals in UE4](https://www.froyok.fr/blog/2019-03-creating-seamless-portals-in-unreal-engine-4/)
- [zachdavids/PortalSystem](https://github.com/zachdavids/PortalSystem)
- [FreetimeStudio/PortalPlugin](https://github.com/FreetimeStudio/PortalPlugin)