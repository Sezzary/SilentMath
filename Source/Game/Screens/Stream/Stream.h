#pragma once

namespace Silent::Game
{
    #define RING_SIZE 23
    #define MOVIE_WAIT 2000
    #define PPW 3 / 2

    // Handlers for movie-related GameStates

    void GameState_MovieIntroFadeIn_Update(void);
    void GameState_MovieIntro_Update(void);
    void GameState_MovieOpening_Update(void);
    void GameState_ExitMovie_Update(void);
    void GameState_DebugMoviePlayer_Update(void); /** @unused debug movie player. Movie to play decided by `LStickLeft`/`LStickRight`. */
    void GameState_MovieIntroAlternate_Update(void);

    // MOVIESYS code

    void open_main(s32 file_idx, s16 num_frames);
    void movie_main(char* file_name, s32 f_size, s32 sector);
}
