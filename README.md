# End of ZZ1 Project (First Year of Engineering School)

To compile the different projects in `clement_perso`:

These projects were made individually to (re)discover SDL2.

- **manipulation_fenetre.c**: Use `-lSDL2 -lSDL2_ttf`
- **figure_geom**: Use `-lSDL2 -lSDL2_ttf`
- **sprite.c**: Use `-lSDL2 -lSDL2_ttf -lSDL2_image`
- **sprite.c** in terminal/debug mode: Use `-DDEBUG`

## Main Project:
Simply run `make` and then `./main.out` inside the `labyrinthe` folder to compile.

This project was done in a group of three.  
During this project, we used **Kruskal's algorithm** to generate a maze that is more or less dense in walls.  
We also explored a wide range of **graph traversal algorithms**: BFS, Dijkstra, A*, and destination search in an unknown graph, whether the destination is known or not.  
We used **SDL2** to create the graphical part of the game.  

All **character/monster sprites** come from this site: [Universal LPC Spritesheet Character Generator](https://liberatedpixelcup.github.io/Universal-LPC-Spritesheet-Character-Generator/#?body=Body_color_light&head=Human_male_light)

The **maze tiles (floor and walls)** were made by Etienne.  
The **intro music** was composed by a relative of one of the group members.  
The **in-game music** was composed by Sasha.
