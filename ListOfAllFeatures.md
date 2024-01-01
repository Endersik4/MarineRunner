<h3>List of all features MarineRunner has, this is described in brief without details, each of these things has many more smaller things in it that make it all work nicely. <br/>
Some of the features may be unclear. I recommend watching the latest demo version for better context. </h3>

<h2> Player </h2>
- Physics-based movement  <br/>
- Crouch <br/>
- Pull up on the edge <br/>
- Slide <br/>
- Sliding down the ramp (speed increases while sliding down the ramp) <br/>
- Wallrun <br/>
- Dash <br/>
- Slow Motion <br/>
- Hook/Swing (force is applied to the player in the direction of the hook) <br/>
- Can use the First Aid Kit <br/>
- Quick Inventory for weapons (1 - first weapon, 2 - second weapon etc) <br/>
- Take Item (resources, weapons etc) <br/>
- Drop Item  <br/>
- Save game (player location, play time, date, inventory etc) <br/>
- Dynamic music depending on events with fade out (menu, combat, exploration music) <br/>
- Pause Menu (with video, keybindings, audio, game settings) <br/>
- Death widget when player died (player can load last save or exit the game) <br/>
- Messages that player can pick up and read  <br/>
- tutorial with customizable text that shows on player screen and disapear after time
<h4> - HUD </h4> 
- Shows current health on progress bar <br/>
- Shows currently holding weapon with ammunition information <br/>
- When you hover over an item, information (name, description, icon) about that item will appear <br/>
- (First Aid Kit, Dash, Slow Motion) information (when to use, how much left, what button to press) <br/>
- Hit, Dash, Slow Motion animation <br/>

<h2> Pause Menu <a href="https://youtu.be/TWsT171ZXYA?t=9"> Showcase </a></h2> 
- Pause Menu with working resume, load game, save game, settings and quit buttons<br/>
<h4> - Load Game </h4>
- List View with all saves  <br/>
- list view entry shows the name of the save, screenshot taken while saving the game, save date, total game time saved <br/>
- The message "Confirm Loading" will appear. <br/>
<h4> - Save Game </h4>
- Saves game when can (cant save while in Air or in Combat) <br/>
- The notification "Saved Game" appear after saving the game <br/>
<h4> - Settings </h4>
- (Game, Audio, Video, Bindings) Settings, Accept, Back to Defaults <br/>
- Every settings (game, audio etc) is in List View and one entry == one setting <br/>
- Any Entry value can be saved to config and is loaded When the game is first launched <br/>
- Entry can be set to: 
<ul>
        <li>Category (centered text with diffrent color) </li>
	<li>Quality (like Low, Med, High, Ultra - 0,1,2,3 value) </li>
	<li>Checkbox (true, false value) and checkbox can be connected to other settings </li>
	<li>Slider value (slider range or number of decimal digits that can be set) </li>
	<li>Key mapping (changed by Key mapping name) </li>
	<li>Set resolution of screen (gets list of possible resolutions)</li>
	<li>Set Window Type (FullScreen, Windowed etc) </li>
</ul>
- Entry value can be applied by:
<ul>
	<li>executing command in console <br/></li>
	<li>sounds (value is saved as volume in given sound class and mix sound class) <br/></li>
	<li>none <br/></li>
</ul>
- Entry can be disabled and grayed out <br/>
<h4> - Quit Game </h4> 
- The message "Confirm quiting" will appear <br/>

<h2> Main Menu </h2>
- Almost the same as the pause menu, but without saving the game and with a new game/continue button

<h2> Weapons </h2>
	- The player can fire the weapon in hipfire or ADS mode. <br/>
	- Shooting mechanic:<br/>
	<ul>
		<li> When the player shoots, the bullet spawns </li>
		<li> The bullet moves based on physics or custom move (you can set how far it can move, at what speed, etc.). </li>
		<li> Camera, Fov and Gun (curves) recoil will play in timeline. Based on what is set </li>
	</ul>
	- ADS is more precise then Hipfire<br/>
	- When shooting camera can go up (its go as far as it was set and then returns to the starting posiiton)<br/>
	- Bullet Casing ejection after the shot<br/>
	- Scope with zoom (when in ADS player can zoom in or out)<br/>
	- Reload (reload the entire magazine or one bullet)<br/>
	- Ammunition based on pickable item<br/>
	- Can spawn more bullets at once (like shotgun)<br/>
	- The weapon can be set to automatic (by holding down the button, the weapon will fire continuously).<br/>
	- When a bullet spawns then a random value from the range is selected for the bullet rotation<br/>
	- The gun can sway while moving the camera or in motion  (diffrent sway for other case)<br/>
	- Animation: Shoot, Reload and empty magazine<br/>
	
<h2> <a href="https://www.youtube.com/embed/dtFB4vfd2Eg"> Albertos </a> (crafting item robot)</h2>
	- It Takes player location and then selects random location within given radius around player location and goes there <br/>
	- When player is near then Albertos looks towards player and player can interact with it:<br/>
	<ul>
		<li> The Crafting Widget is visible and player can interact with it</li>
		<li> On Left Side Crafting widget shows all items that player has (weapons, ammo, first aid kits...) </li>
		<li> On Upper Right Side there is TileView with resources that currently player has (wood, metal, steel...)</li>
		<li> On Middle Right Side there is information about item that is currently selected to craft. Name of item, description, time to craft, amount</li>
		<li> On Lower Right Side there is requirements of resources that player has to have to create the item</li>
		<li> In The Center there is image with item icon that is currently selected, below that player can choose item amount multiplier (2x, 4x...), 
		after that there is craft button</li>
		<li> Item can be changed by clicking on arrows (left or right side)</li>
	</ul>
	- When player clicks craft button:
		<ul>
			<li> </li>
		</ul>
	- Can play random sounds while moving <br/>
	- Diffrent "footsteps" sounds depending on speed of robot <br/>
	- Everything is animated 
<h2> Enemies </h2>

<h2> Objects </h2>
<h4> Elevator: </h4>
- Player can call elevator from outside and while elevator goes to the player, "Wait For Elevator" animation will play on widget
- When Player is in the elevator, he can choose floor to go to and after elevator starting animation it wll go to the floor that player chose
- Select Floor on widget is entry in Tile View and can be many of these, Select Floor entry can be also set up as blocked then player cant go to this floor
- Eleveator can require pin code to enter
- When player selects floor that he currently is then it will open elevator doors
- Everything is animated in Widget Blueprint
<h4> Doors </h4>
- Can be open from two sides by clicking on panel (3D Widget) 
- Door can require pin code to open
- After opening the door, close door timer will playe

