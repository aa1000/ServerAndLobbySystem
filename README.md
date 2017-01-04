# ServerAndLobbySystem
Server and Lobby System WIP test in Unreal Engine 4 C++ and UMG using the OnlineSubsystem

Quick features video:
https://youtu.be/T9zm8DYGDtM

Features:
* Play over Lan or Steam
* Can set server display name with error checking
* Set any number of players you want to be able to join
* Make a server protected by a password
* potentially add any other attribute wanted to the session info/setting
* Can show all of the above in a server list
* players can't join a full game

unfortunately, so far all of this is static meaning that you only get it once when you search for the session then it gets stored so if the setting changes you won't notice unless you refresh the server list but I'll try to make it update in real-time.

A full Lobby where you can:
* Freely change some of the session settings
* Set Game settings that other players can see in Server List
* Chatbox
* Kick players
* Invite Players Online
* Accept Invites from players online
* Show steam friend list
* Show the Player's Steam Avatar if Playing online

This is a C++ test/learning project so it is far from perfect or optimized

I also know a lot of this is in the Advanced Sessions Plugin already but I'm trying to learn how to implement it in C++ myself
