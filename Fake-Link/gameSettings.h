#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

struct gameSettings{
    int row = 8;
    int col = 10;
    int numTypes = 3;

    bool character = false;
    int maxTurns = 3;
};

#endif // GAMESETTINGS_H
