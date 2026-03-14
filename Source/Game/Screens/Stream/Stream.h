#pragma once

namespace Silent::Game
{
    #define RING_SIZE 23
    #define MOVIE_WAIT 2000
    #define PPW 3 / 2

    // Handlers for movie-related GameStates

    void GameState_MovieIntroFadeIn_Update();
    void GameState_MovieIntro_Update();
    void GameState_MovieOpening_Update();
    void GameState_ExitMovie_Update();
    void GameState_DebugMoviePlayer_Update(); /** @unused debug movie player. Movie to play decided by `LStickLeft`/`LStickRight`. */
    void GameState_MovieIntroAlternate_Update();

    // MOVIESYS code

    void open_main(s32 file_idx, s16 num_frames);
    bool movie_main(const std::string& file_name, s32 f_size, s32 sector);
}
