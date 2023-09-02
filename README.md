# Rules of the Game of Cabeza

## Rules

Cabeza is a two player strategy game played on a 10x10 board. Each player has 5 different pieces in the game at all times.
Except for the _cabeza_, the rest of the pieces consist of integer-sided rectangular prisms which are always aligned to the board grid (called block pieces).

Players (red and blue) take turns, starting with red. In each turn a player has to choose a single piece and move it once or twice according to valid move patterns.
All block pieces move by rotating along one of its edges that touch the board, effectivly "rolling" from one position to another.

All pieces can be rotated once or twice per turn, except for the _Opa_ (2x2x2) piece which can only be rolled once.

Specifically, the block pieces are named:

- _Mini_   1 x 1 x 1  
- _Flaco_  2 x 1 x 1
- _Chato_  2 x 2 x 1 
- _Opa_    2 x 2 x 2 

Finally the last piece is the _Cabeza_. This piece moves by sliding to a neighbouring square, including its four diagonal squares. (Similar to the king in chess). This piece can slide once or twice per turn.
If a diagonal slide motion is blocked by the edge of a piece, it can pass through. But if a diagonal slide requires cutting through the edges of two pieces, then it cannot pass through.

[INSERT CABEZA EXAMPLE]

Null moves aren't allowd for any piece.
Moves that result in a piece landing on a square occuped by another piece of either player are no allowd. Except for a block piece landing on the opponent's _Cabeza_ piece (see Winning)

The initial starting position is as follows:

[INSERT STARTING POSITION]

## Winning

A player wins by performing a move that:
  - Places a block piece on top of the opponents _Cabeza_ piece

  - Lands the players cabeza to the oponents home row.


# Notation

## Movements

A move is represented by a 2 to 5 letter string. The first letter representing the piece:

- c/C : Cabeza
- m/M : Mini
- f/F : Flaco
- h/H : Chato
- o/O : Opa
- Uppercase letters represent red pieces and lowercase letters represent blue pieces.

Follow by a combination of 'n', 's', 'e', 'w', representing the cardinal directions (north, south, east, west), representing the directions of rolling or sliding the piece performs.
The directions are defined for the board orientad having the red starting pieces on the south, and blue starting pieces on the north.

For a _Cabeza_, a diagonal move can be represented by a combination of North/South with East/West

For _Cabeza_ and _Mini_ pieecs where move order is irrelevant, north (n) and south (s) are always written before east (e) and west (w).

For instance:

- Fne : Represents the red _Flaco_ piece rolling north once and then rolling east.
- ms : Represents the blue _Mini_ piece rolling south once
- Cnne : Represents the red _cabeza_ sliding northeast once and north once (or the other way around, move order is irrelevant for cabeza)
- Fen : Represents the red _Flaco_ piece rolling east once and then north. Note, it is NOT the same move as Fne.
- ow: Represents the blue _Opa_ rolling west once.


# Software

This is my hobby project to create a cabeza game engine. Capable of playing the game competitively against a human.

Commands:

- "draw"/"d"

    Draws board on screen using characters

- "move"/"m" [move]
  
  Plays a move for the current active player and switches turn

- "turn"/"t" [player]
  
    If no argument is passed, switches the turn. If "red" or "r" is passed, switches to red. If "blue" or "b" is passed, switches to blue.

- "search"/"s"
  
    Performs a search to find the best move for the current player.

- "play"/"P"
  
    Same as "search" but after search is completed, it plays the best move and switches turns.

- "fen" [fen]
  
    If no outputs are passed, it prints a FEN-like string representing the current game state. If a valid FEN string is passed, it sets the current state to it.

- "undo"/"u"
  
    Reverts last performed move, either by a move command or play command

- "reset"
  
    Resets game to starting position

- "quit"
  
    Exits program



