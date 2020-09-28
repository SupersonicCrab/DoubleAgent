Thank you for playtesting project spy!

Launching:
In this folder you should see the "WindowsNoEditor" folder which contains "ProjectSpy.exe" which is what you use to launch the game
You will also see a web link bringing you to a google form webpage that we use for bug reports and feedback

Hosting a multiplayer game:
- Open game to main menu
- Click the plus(+) icon next to your steam profile picture in the top right corner
- This should open up a list of your friends with a corresponding "invite" button
- The game is only built supports 2 players, the host and 1 client: please keep that in mind
- After your friend joins, you should receive confirmation in the top left corner in red that your friend has connected to your session
- Click "New Game" and you'll join the game with your friend

Joining a multiplayer game:
- After your friend has hosted and invited you, you should have an invite from them on steam
- Make sure you have Double Agent open, and then accept the invite from the steam overlay
- After you've successfully connected, you'll lose the "New Game" and "Load Game" buttons on your screen
- Wait for the host to start the game

In this build:
- Starting room
- Roaming Guards room
  - This is meant to showcase our AI functionality and part of our win condition.
- Security Room
  - This is meant to showcase our Security system functionality with AI integration
  - This is also used to showcase the basic functionality of one of the players equipment: The Camera Hack Tool

Known issues:
- Inventory tooltip sometimes has a visual bug that causes it to flicker
- Takedown animation does not always work over the network & is dependant on framerate
- Inventory can glitch if you drag it off screen
- Inventory disappears if you drag it and let go while it's hovering over another UI widget (i.e. health bar)
- If you get too far away from an inventory you're looting, it will not disappear
- Using tab will not work to close the inventory if you have the inventory window in focus (it was the last thing you clicked on)
- Main menu will not display profile picture for joining client. Will still display it in single player mode.
- Inventory sometimes might just not work. It'll appear and the mouse will appear, but you can't interact with the inventory. If this happens, just restart the level and it should be fine

Troubleshooting:
If you have troubles launching the game, your computer might be missing some prerequisites to run unreal engine, these can be
found in "WindowsNoEditor\Engine\Extras\Redist\en-us", run "UE4PrereqSetup_x64.exe" and you should be good to go