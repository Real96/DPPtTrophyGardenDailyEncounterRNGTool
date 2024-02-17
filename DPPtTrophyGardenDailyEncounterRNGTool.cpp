#include <iostream>
#include <math.h>
#include <string_view>
#include <array>
#include <regex>

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
void sanitizeInput(const string output, T &value, const T lowLimit, const T highLimit) {
    while ((cout << output) && (!(cin >> value) || (value < lowLimit || value > highLimit))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

bool sanitizeYesNoInput(const string output) {
    string yesNoAnswer;
    const regex yesNoRegex("^[nyNY]$");

    while ((cout << output) && (!(cin >> yesNoAnswer) || !regex_match(yesNoAnswer, yesNoRegex))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    return toupper(yesNoAnswer[0]) == 'Y';
}

void sanitizeHexInput(uint32_t &seed) {
    string stringSeed;
    const regex hexRegex("^[0-9a-fA-F]{1,8}$");

    while ((cout << "Insert the initial seed: ") && (!(cin >> stringSeed) || !regex_match(stringSeed, hexRegex))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    seed = stoul(stringSeed, nullptr, 16);
}

void getRNGInput(short &encounter, uint32_t &seed, unsigned long &advances, bool &knownSeedFlag) {
    sanitizeInput<short>("Insert the wanted encounter index: ", encounter, 1, 16);
    knownSeedFlag = sanitizeYesNoInput("Do you know the initial seed? (y/n) ");

    if (knownSeedFlag) {
        sanitizeHexInput(seed);
        sanitizeInput<unsigned long>("Insert the current advances: ", advances, 0, ULONG_MAX);
    }
}

uint32_t LCRNG(const uint32_t seed) {
    return 0x41C64E6D * seed + 0x6073;
}

uint32_t advanceRNG(uint32_t &seed, const unsigned long n = 1) {
    for (unsigned long i = 0; i < n; i++) {
        seed = LCRNG(seed);
    }

    return seed;
}

uint8_t getUpper4Bit(const uint32_t seed) {
    return seed >> 28;
}

bool isWantedEncounterCheck(uint32_t &seed, const short encounter) {
    return getUpper4Bit(advanceRNG(seed)) == encounter - 1;
}

void findTrophyGardenPokemon(const short encounter, uint32_t seed, unsigned long advances) {
    for (;; advances++, advanceRNG(seed)) {
        uint32_t tempSeed = seed;

        if (!isWantedEncounterCheck(tempSeed, encounter)) {
            continue;
        }

        printf("\n\nTarget seed: %08X | Target advances: %lu\n\n------------------------------------------------\n\n", seed, advances);
        return;
    }
}

void findTrophyGardenSeed(const short encounter) {
    const short hour = 24, maxDelay = 10000;
    short minDelay;
    unsigned long maxAdvances;

    sanitizeInput<short>("Insert the min delay: ", minDelay, 600, 9999);
    sanitizeInput<unsigned long>("Insert the max advances: ", maxAdvances, 1, ULONG_MAX);

    for (short ab = 0; ab < 256; ab++) {
        for (short cd = 0; cd < hour; cd++) {
            for (short efgh = minDelay; efgh < maxDelay; efgh++) {
                const uint32_t seed = ((ab << 24) | (cd << 16)) + efgh;
                uint32_t tempSeed = seed;

                for (unsigned long advances = 0; advances < maxAdvances; advances++) {
                    if (!isWantedEncounterCheck(tempSeed, encounter)) {
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
    bool knownSeedFlag;

    while (true) {
        printTropyGardenDailiyEncounters();
        getRNGInput(encounterIndex, initialSeed, currentAdvances, knownSeedFlag);

        if (knownSeedFlag) {
            advanceRNG(initialSeed, currentAdvances);
            findTrophyGardenPokemon(encounterIndex, initialSeed, currentAdvances);
            continue;
        }

        findTrophyGardenSeed(encounterIndex);
    }
}
