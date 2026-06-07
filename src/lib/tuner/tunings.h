#include <avr/io.h>
#include <lib/sound/pitches.h>

#ifndef TUNER_H
#define TUNER_H

// --- Definizione struttura accordatura ---

struct Tuning {
    const uint16_t* strings;
    const char* const* names;
    uint8_t num_strings;
    const char* label;
};

// --- Array di frequenze per ogni accordatura ---
static const uint16_t STD_GTR[]        = {E2, A2, D3, G3, B3, E4};
static const uint16_t HALF_STEP_DOWN[] = {DS2, GS2, CS3, FS3, AS3, DS4};
static const uint16_t DROP_D[]         = {D2, A2, D3, G3, B3, E4};
static const uint16_t OPEN_G[]         = {D2, G2, D3, G3, B3, D4};
static const uint16_t OPEN_D[]         = {D2, A2, D3, FS3, A3, D4};
static const uint16_t OPEN_E[]         = {E2, B2, E3, GS3, B3, E4};
static const uint16_t BASS_STD[]       = {E1, A1, D2, G2};
static const uint16_t BASS_DROP_D[]    = {D1, A1, D2, G2};
static const uint16_t BASS_5_STRING[]  = {B0, E1, A1, D2, G2};
static const uint16_t VIOLIN_STD[]     = {G3, D4, A4, E5};
static const uint16_t UKULELE_STD[]    = {G4, C4, E4, A4};

// --- Array di nomi per ogni accordatura ---
static const char* _NAMES_STD_GTR[]        = {"E2", "A2", "D3", "G3", "B3", "E4"};
static const char* _NAMES_HALF_STEP_DOWN[] = {"Eb2","Ab2","Db3","Gb3","Bb3","Eb4"};
static const char* _NAMES_DROP_D[]         = {"D2", "A2", "D3", "G3", "B3", "E4"};
static const char* _NAMES_OPEN_G[]         = {"D2", "G2", "D3", "G3", "B3", "D4"};
static const char* _NAMES_OPEN_D[]         = {"D2", "A2", "D3", "F3", "A3", "D4"};
static const char* _NAMES_OPEN_E[]         = {"E2", "B2", "E3", "G3", "B3", "E4"};
static const char* _NAMES_BASS_STD[]       = {"E1", "A1", "D2", "G2"};
static const char* _NAMES_BASS_DROP_D[]    = {"D1", "A1", "D2", "G2"};
static const char* _NAMES_BASS_5_STRING[]  = {"B0", "E1", "A1", "D2", "G2"};
static const char* _NAMES_VIOLIN_STD[]     = {"G3", "D4", "A4", "E5"};
static const char* _NAMES_UKULELE_STD[]    = {"G4", "C4", "E4", "A4"};

// --- Tabella centrale di tutte le accordature ---

static const Tuning TUNINGS[] = {
    {STD_GTR,        _NAMES_STD_GTR,        6, "G_Std"  },
    {HALF_STEP_DOWN, _NAMES_HALF_STEP_DOWN, 6, "G_HDown"},
    {DROP_D,         _NAMES_DROP_D,         6, "G_DropD"},
    {OPEN_G,         _NAMES_OPEN_G,         6, "G_OpenG"},
    {OPEN_D,         _NAMES_OPEN_D,         6, "G_OpenD"},
    {OPEN_E,         _NAMES_OPEN_E,         6, "G_OpenE"},
    {BASS_STD,       _NAMES_BASS_STD,       4, "B_Std"  },
    {BASS_DROP_D,    _NAMES_BASS_DROP_D,    4, "B_DropD"},
    {BASS_5_STRING,  _NAMES_BASS_5_STRING,  5, "B_5Str" },
    {VIOLIN_STD,     _NAMES_VIOLIN_STD,     4, "V_Std"  },
    {UKULELE_STD,    _NAMES_UKULELE_STD,    4, "U_Std"  },
};

#define NUM_TUNINGS (sizeof(TUNINGS) / sizeof(TUNINGS[0]))

#endif