#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

struct gameSettings{
    int row = 6;
    int col = 6;
    int numTypes = 4;

    bool character = false;
    int maxTurns = 2;
};

#endif // GAMESETTINGS_H
