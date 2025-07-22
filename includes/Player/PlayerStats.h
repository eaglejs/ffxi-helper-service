#pragma once

/**
 * Structure to hold player base stats from FFXI
 * Based on Elite API PlayerStats structure
 */
struct PlayerStats
{
    short Strength;
    short Dexterity;
    short Vitality;
    short Agility;
    short Intelligence;
    short Mind;
    short Charisma;

    // Default constructor
    PlayerStats()
        : Strength(0), Dexterity(0), Vitality(0), Agility(0),
          Intelligence(0), Mind(0), Charisma(0) {}

    // Constructor with values
    PlayerStats(short str, short dex, short vit, short agi, short intel, short mnd, short chr)
        : Strength(str), Dexterity(dex), Vitality(vit), Agility(agi),
          Intelligence(intel), Mind(mnd), Charisma(chr) {}
};
