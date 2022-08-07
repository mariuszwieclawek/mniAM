# mniAM
Project as part of the subject "Microcontroller Applications".

mniAM is a turn based game. At the beginning of the game, players materialize on the board and get information about each other, i.e. their positions and the initial amount of hp. In addition, each player gets information about where the transistors (i.e. food) are located.

Then each player is polled for movement. In response, each player sends back the value of the angle indicating the direction of movement. After receiving information from all players, the game engine:
- updates the positions of each player on the board,
- checks for possible collisions of players with transistors or with other players and, if it detects such ones, updates players' hp, sends information about the changes   to players,
- then the next turn begins.

The game ends when:
- Only one player will remain on the board, that is, the winner or
- the time limit is exceeded - then the player with the most hp is the winner

![image](https://user-images.githubusercontent.com/57256517/183287410-b75a9e2c-086f-4c50-bb2d-cf2e083fbfd8.png)
