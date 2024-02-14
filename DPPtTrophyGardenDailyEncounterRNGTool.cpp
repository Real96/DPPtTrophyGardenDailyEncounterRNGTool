#include <iostream>
#include <string_view>
#include <array>
#include <regex>
#include <math.h>
#include <cstdint>

using namespace std;

void printTropyGardenDailiyEncounters() {
    static constexpr array<string_view, 16> pokemonNames{ "Eevee", "Bonsly", "Happiny", "Meowth", "Cleffa", "Clefairy",
                                                          "Igglybuff", "Plusle", "Jigglypuff", "Porygon (DP) / Ditto (Pt)",
                                                          "Castform", "Minun", "Mime Jr.", "Marill", "Chansey", "Azurill" };

    cout << "Trophy Garden encounters:\n\n";

    for (short i = 0; i < 16; i++) {
        cout << i + 1 << " " << pokemonNames[i] << "\n";
    }

    cout << "\n\n";
}

template <typename T>
void sanitizeInput(const string &output, T &index, T lowLimit, T highLimit) {
    while ((cout << output) && (!(cin >> index) || (index < lowLimit || index > highLimit))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

bool sanitizeYesNoInput(const string &output) {
    string yesNoAnswer;
    regex yesNoRegex("^[nyNY]$");

    while ((cout << output) && (!(cin >> yesNoAnswer) || !regex_match(yesNoAnswer, yesNoRegex))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    return toupper(yesNoAnswer[0]) == 'Y';
}

void sanitizeHexInput(uint32_t &seed) {
    string stringSeed;
    regex hexRegex("^[0-9a-fA-F]{1,8}$");

    while ((cout << "Insert the initial seed: ") && (!(cin >> stringSeed) || !regex_match(stringSeed, hexRegex))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    seed = stoul(stringSeed, nullptr, 16);
}

void getRNGInput(uint32_t &seed, unsigned long &advances, bool &researchFlag) {
    researchFlag = sanitizeYesNoInput("Do you know the initial seed? (y/n) ");

    if (researchFlag) {
        sanitizeHexInput(seed);
        sanitizeInput<unsigned long>("Insert the current advances: ", advances, 0, ULONG_MAX);
    }
}

uint32_t LCRNG(uint32_t seed) {
    return 0x41C64E6D * seed + 0x6073;
}

void advanceRNG(uint32_t &seed, unsigned long n = 1) {
    for (unsigned long i = 0; i < n; i++) {
        seed = LCRNG(seed);
    }
}

uint8_t getUpper4Bit(uint32_t seed) {
    return seed >> 28;
}

bool isWantedEncounterCheck(uint32_t seed, short index) {
    return getUpper4Bit(seed) == index - 1;
}

void findTrophyGardenPokemon(short index, uint32_t seed, unsigned long advances) {
    for (;; advances++) {
        uint32_t tempSeed = LCRNG(seed);

        if (!isWantedEncounterCheck(tempSeed, index)) {
            advanceRNG(seed);
            continue;
        }

        printf("\n\nTarget seed: %08X | Target advances: %lu\n\n------------------------------------------------\n\n", seed, advances);
        return;
    }
}

void findTrophyGardenSeed(short index) {
    const short hour = 24, maxDelay = 10000;
    short minDelay;
    unsigned long maxAdvances;

    sanitizeInput<short>("Insert the min delay: ", minDelay, 600, 9999);
    sanitizeInput<unsigned long>("Insert the max advances: ", maxAdvances, 1, ULONG_MAX);

    for (short ab = 0; ab < 256; ab++) {
        for (short cd = 0; cd < hour; cd++) {
            for (short efgh = minDelay; efgh < maxDelay; efgh++) {
                uint32_t seed = ((ab << 24) | (cd << 16)) + efgh;
                uint32_t tempSeed = seed;

                for (unsigned long advances = 0; advances < maxAdvances; advances++) {
                    uint32_t tempSeed2 = LCRNG(tempSeed);

                    if (!isWantedEncounterCheck(tempSeed2, index)) {
                        advanceRNG(tempSeed);
                        continue;
                    }

                    printf("\n\nTarget seed: %08X | Target advances: %lu\n\n------------------------------------------------\n\n", seed, advances);
                    return;
                }
            }
        }
    }
}

int main() {
    short encounterIndex;
    uint32_t initialSeed;
    unsigned long currentAdvances;
    bool seedResearchFlag;

    while (true) {
        printTropyGardenDailiyEncounters();
        sanitizeInput<short>("Insert the wanted encounter index: ", encounterIndex, 1, 16);
        getRNGInput(initialSeed, currentAdvances, seedResearchFlag);

        if (seedResearchFlag) {
            advanceRNG(initialSeed, currentAdvances);
            findTrophyGardenPokemon(encounterIndex, initialSeed, currentAdvances);
            continue;
        }

        findTrophyGardenSeed(encounterIndex);
    }
}
