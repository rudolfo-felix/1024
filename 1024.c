/////////////////////////////////////////////////////////////////////////////////////
/////////////////////// INSTITUTO SUPERIOR TÉCINCO /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/////Jogo 1024 - Projeto Intermédio da cadeira de Progamação - MEEC 16/17/////////
/////////////////////////////////////////////////////////////////////////////////
//////////Trabalho realizado por: Rudolfo Félix ///// nº87114 //////////////////
///////////////////////////////////////////////////////////////////////////////
//////código de compilação no terminal ->
//////gcc file_name.c -g -I/usr/local/include -Wall -pedantic -std=c99 -L/usr/local/lib -lm -lSDL2 -lSDL2_ttf -lSDL2_image -o 1024
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 650        // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 200       // initial position of the window: x
#define WINDOW_POSY 200       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size
#define MAX_BOARD_POS 11      // maximum size of the board
#define MAX_LEVELS 23
#define MARGIN 5


// declaration of the functions related to graphical issues
void InitEverything(int , int , TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer ** );
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
void RenderTable(int, int *, int *, TTF_Font *, SDL_Surface **, SDL_Renderer *);
void RenderBoard(int [][MAX_BOARD_POS], SDL_Surface **, int, int, int, SDL_Renderer *);
void RenderStats( SDL_Renderer *, TTF_Font *, TTF_Font *, int *, int *, char [STRING_SIZE], time_t);
void LoadValues(SDL_Surface **);
void UnLoadValues(SDL_Surface **);

/////////////////////////////////////////////////
////////////  FUNÇÕES CRIADAS ///////////////////
/////////////////////////////////////////////////

//função de leitura de parametros introduzidos pelo utilizador: tamanho do tabuleiro, nome e dificuldade do jogo
void read_parameteres(char /*username*/[STRING_SIZE], char [STRING_SIZE], int *, int *);
//função para pôr o tabuleiro sem peças
void clear_board(int [][MAX_BOARD_POS], int [][MAX_BOARD_POS], int *);
//função para dar random entre as peças 2¹ e 2² e colocá-las em random posições
void random_2 (int [][MAX_BOARD_POS], int *);
//função para dar random de uma peça entre 2¹ e 2² e colocá-la num espaço
void random_1 (int [][MAX_BOARD_POS], int *);
//função da tecla para mover as peças para a direita
void right_key(int [][MAX_BOARD_POS], int *);
//função da tecla para mover as peças para a esquerda
void left_key(int [][MAX_BOARD_POS], int *);
//função da tecla para mover as peças para a cima
void down_key(int [][MAX_BOARD_POS], int *);
//função da tecla para mover as peças para a cima
void up_key(int [][MAX_BOARD_POS], int *);
//soma para cima
void up_soma(int [][MAX_BOARD_POS], int*, int*);
//soma para baixa
void down_soma(int [][MAX_BOARD_POS], int*, int*);
//soma para a esquerda
void left_soma(int [][MAX_BOARD_POS], int*, int*);
//soma para a direita
void right_soma(int [][MAX_BOARD_POS], int*, int*);
//copia da matriz atual
void board_copy(int [][MAX_BOARD_POS], int [][MAX_BOARD_POS], int *, int *, int *);
//associa a matriz da jogada anterior à matriz na função undo_play
void change_board(int [][MAX_BOARD_POS], int [][MAX_BOARD_POS], int *);
//undo de uma jogada
void undo_play(int [][MAX_BOARD_POS], int [][MAX_BOARD_POS], int *, int *, int *);
//função que avalia que há movimento
int check_mov(int [][MAX_BOARD_POS], int [][MAX_BOARD_POS], int *);
//função grafica da vitória
void victory( SDL_Renderer *, TTF_Font *, int *, int *, int [][MAX_BOARD_POS], int *);
//função que avalia se há derrota
int check_defeat(int *bd_size, int board[][MAX_BOARD_POS]);
//função grafica da derrota
void defeat( SDL_Renderer *, TTF_Font *, int *);
//função que apresenta as estatisticas do jogos num ficheiro .txt
void statistics(char [STRING_SIZE], int *, time_t, int [][MAX_BOARD_POS], int *, int *);
//função 1 do save, escreve num ficheiro .txt todos os parametros do jogo
void write_save(char [STRING_SIZE], int [][MAX_BOARD_POS], int *, int *, int *, int *, time_t);
//função 2 do save, lẽ um ficheiro .txt todos os parametros do jogo
void read_save(char [STRING_SIZE], int [][MAX_BOARD_POS], int *, int *, int *, int *, int *, long int * );


// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Rudolfo Felix";
const char myNumber[] = "IST187114";

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( int argc, char* args[] )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    SDL_Surface *array_of_numbers[MAX_LEVELS], *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px, board_size_px, board_pos = 4;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};//matriz atual
    int copy_board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};//matriz da jogada anterior
    int val_board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};//matriz da jogada anterior para validação

    time_t timer = 0, t_pc = 0, t_atual = 0;//variaveis usadas para contar o tempo de cada jogo
    long int time_saved = 0;//tempo do jogo que  guardado pela função write_save
    int zero_one = 0, u = 0, ret = 0;//variavel zero_one determina a continuação do ultimo jogo ou de novo jogo. a variavel u é a flag do undo. a variavel ret é explicada mais abaixo.
    int start = 0, i = 0, game = 0;//flags para iniciar os movimentos das peças e tempo do jogo(start), o numero de jogos por cada utilizador(game). a variavel i é explicada mais abaixo
    int bd_size = 0, difficulty = 0, add = 0, last_add = 0;//tamanho da tabuleiro, dificuldade do jogo, pontuação atual, pontuação da jogada anterior, respetivamente.
    char username[STRING_SIZE] = {0};//nome do utilizador
    char str[STRING_SIZE] = {0};//string default usada para a leitura de alguns parametros

    //inicio do jogo
    //mensagem do inicio do jogo para o utilizador escolher se pretende continuar o ultimo jogo ou fazer um novo jogo
    printf("\n\n      Welcome to 1024.\n\nIf is your first time playing this game press 0(zero).\nIf it isn't the case and if you want to continue to play the last game press 1.\n\n");

    do{
      printf("Press 0 or 1.\n");
      fgets(str, STRING_SIZE, stdin);
      ret = sscanf(str, "%d", &(zero_one));//a variavel ret valida se o é introduzido um algarismo
    }while (ret != 1 || zero_one < 0 || zero_one > 1 );

    if(zero_one == 0)
    {
      read_parameteres(username, str, &bd_size, &difficulty);
      board_pos = bd_size; //o board_pos é que determina o tamanho do tabuleiro, por isso temos de igualar ao valor introduzido pelo utilizador
      clear_board(board, val_board, &bd_size);
    }
    else if (zero_one == 1)
    {
      read_save(username, board, &game, &bd_size, &difficulty, &add, &start, &time_saved);//esta função vai ler introduzir todos os parametros no jogo e é possivel começar logo a jogar,
      board_pos = bd_size;//////////////////////////////////////////////////////////não é preciso clicar 'n' para mover as peças ou começar o tempo
    }

    t_pc = time(NULL); //tempo do computador

	  // initialize graphics
	  InitEverything(width, height, &serif, imgs, &window, &renderer);
    // loads numbers as images
    LoadValues(array_of_numbers);

  	while( quit == 0 )
    {
      if(start == 1) //inicia a contagem do tempo se for pressionada a tecla n
      {
        t_atual = time(NULL); //tempo atual no jogo
        timer = t_atual - t_pc + time_saved; //t_atual - t_pc;//contagem do tempo + o tempo guardado
      }

        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
			    if( event.type == SDL_QUIT )//esta tecla é a cruz presente no canto superior direito da janela do programa que fecha a janela
          {
            statistics(username, &add, timer, board, &bd_size, &game);
            write_save(username, board, &game, &bd_size, &difficulty, &add, timer);
            quit = 1;
          }
          else if ( event.type == SDL_KEYDOWN )
          {
            switch ( event.key.keysym.sym )
            {
              case SDLK_n: //se carregar na letra n (minúscula), começa um novo jogo

              if(i > 0)
              {
                statistics(username, &add, timer, board, &bd_size, &game);
              }

              //início do jogo
              clear_board(board, val_board, &bd_size);//o tabuleiro fica com todas as peças a zero
              random_2(board, &bd_size);//disposição de duas peças random
              //
              time_saved = 0;//reset do tempo guardado
              timer = 0;//reset do tempo do jogo
              t_pc = time(NULL);//tempo atual do computador no momento em que é clicado 'n'

              add = 0;//reset da pontuação
              u = 0;//reset da flag do undo
              start = 1;//flag de movimento e tempo ativada

              i++;//adicionei esta variavel para que nao seja escrito um ficheiro de estatisticas de um jogo que ainda nao começou
              game++;//variavel que conta o número de jogos

              break;

					    case SDLK_UP:
              if (start == 1 )
              {
                board_copy(board, val_board, &bd_size, &last_add, &add);

              //movimento, soma, movimento
                up_key(board, &bd_size);
                up_soma(board, &bd_size, &add);
                up_key(board, &bd_size);

                check_mov(board, val_board, &bd_size);//deteta que o movimento foi válida, idem para as teclas down, right e left

                if(check_mov(board, val_board, &bd_size) == 0)//se o movimento é valido, é gerada uma peça random e matriz do undo é a que foi copiada,  idem para as teclas down, right e left
                {
                  random_1 (board, &bd_size);
                  change_board(copy_board, val_board, &bd_size);
                }

                victory( renderer, serif, &difficulty, &bd_size, board, &start);//mensagem de vitoria,  idem para as teclas down, right e left

                check_defeat(&bd_size, board);//deteta se há derrota ou nao,  idem para as teclas down, right e left
                if (check_defeat(&bd_size, board) == 1)
                {
                  defeat( renderer, serif, &start);
                }
                u++;
              }
              break;

					    case SDLK_DOWN:
              if (start == 1 )
              {
                board_copy(board, val_board, &bd_size, &last_add, &add);

              //movimento, soma, movimento
                down_key(board, &bd_size);
                down_soma(board, &bd_size, &add);
                down_key(board, &bd_size);

                check_mov(board, val_board, &bd_size);

                if(check_mov(board, val_board, &bd_size) == 0)
                {
                  random_1 (board, &bd_size);
                  change_board(copy_board, val_board, &bd_size);
                }

                victory( renderer, serif, &difficulty, &bd_size, board, &start);

                check_defeat(&bd_size, board);
                if (check_defeat(&bd_size, board) == 1)
                {
                  defeat( renderer, serif, &start);
                }
                u++;
              }
              break;

              case SDLK_LEFT:
              if (start == 1 )
              {
                board_copy(board, val_board, &bd_size, &last_add, &add);

                //movimento, soma, movimento
                left_key(board, &bd_size);
                left_soma(board, &bd_size, &add);
                left_key(board, &bd_size);

                check_mov(board, val_board, &bd_size);

                if(check_mov(board, val_board, &bd_size) == 0)
                {
                  random_1 (board, &bd_size);
                  change_board(copy_board, val_board, &bd_size);
                }

                victory( renderer, serif, &difficulty, &bd_size, board, &start);

                check_defeat(&bd_size, board);
                if (check_defeat(&bd_size, board) == 1)
                {
                  defeat( renderer, serif, &start);
                }
                u++;
              }
              break;

		    			case SDLK_RIGHT:
              if (start == 1 )
              {
                board_copy(board, val_board, &bd_size, &last_add, &add);

                //movimento, soma, movimento
                right_key(board, &bd_size);
                right_soma(board, &bd_size, &add);
                right_key(board, &bd_size);

                check_mov(board, val_board, &bd_size);

                if(check_mov(board, val_board, &bd_size) == 0)
                {
                  random_1 (board, &bd_size);
                  change_board(copy_board, val_board, &bd_size);
                }

                victory( renderer, serif, &difficulty, &bd_size, board, &start);

                check_defeat(&bd_size, board);
                if (check_defeat(&bd_size, board) == 1)
                {
                  defeat( renderer, serif, &start);
                }
                u++;
              }
              break;

              case SDLK_q: //se carregar na letra q (minúscula), a janela do jogo fecha
              statistics(username, &add, timer, board, &bd_size, &game);
              write_save(username, board, &game, &bd_size, &difficulty, &add, timer);
              quit = 1;
              break;

              case SDLK_u: //se carregar na letra u (minúscula), vamos voltar à jogada anterior
              if ((start == 1) & (u > 0))
              {
                undo_play(board, copy_board, &bd_size, &last_add, &add);//é introduzida a matriz da jogada anterior
              }
              break;

					    default:
						  break;
            }
          }
        }

        // render game table
        RenderTable( board_pos, &board_size_px, &square_size_px, serif, imgs, renderer);
        // render board
        RenderBoard(board, array_of_numbers, board_pos, board_size_px, square_size_px, renderer);

        //escrita do nome do jogador, pontuacao, tempo e nivel de dificuldade do jogo
        RenderStats(renderer, serif, serif, &difficulty, &add, username, timer);

        //mensagem de vitória
        victory( renderer, serif, &difficulty, &bd_size, board, &start);

        //mensagem de derrota
        if (check_defeat(&bd_size, board) == 1)
        {
          defeat( renderer, serif, &start);
        }


        // render in the screen all changes above
        SDL_RenderPresent(renderer);
    	// add a delay
		SDL_Delay( delay );
    }

    // free memory allocated for images and textures and closes everything including fonts
    UnLoadValues(array_of_numbers);
    TTF_CloseFont(serif);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
	  SDL_DestroyWindow(window);
	  SDL_Quit();
	return EXIT_SUCCESS;
}


/**
 * LoadCards: Loads all images with the values
 * \param _array_of_numbers vector with all loaded numbers
 */
void LoadValues(SDL_Surface **_array_of_numbers)
{
    int i;
    char filename[STRING_SIZE];

     // loads all images to an array
    for ( i = 0 ; i < MAX_LEVELS; i++ )
    {
        // create the filename !
        sprintf(filename, ".//numbers//value_%02d.jpg", i+1);
        // loads the image !
        _array_of_numbers[i] = IMG_Load(filename);
        // check for errors: deleted files ?
        if (_array_of_numbers[i] == NULL)
        {
            printf("Unable to load image: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
}


/**
 * UnLoadCards: unloads all values images of the memory
 * \param _array_of_numbers vector with all loaded image numbers
 */
void UnLoadValues(SDL_Surface **_array_of_numbers)
{
    // unload all cards of the memory: +1 for the card back
    for ( int i = 0 ; i < MAX_LEVELS; i++ )
    {
        SDL_FreeSurface(_array_of_numbers[i]);
    }
}

/**
* RenderBoard: renders the board
* \param _board multidimensional array with the board numbers
* \param _array_of_numbers vector with all loaded images
* \param _board_pos number of positions in the board
* \param _board_size_px size of the board in pixels
* \param _square_size_px size of each square
* \param _renderer renderer to handle all rendering in a window
*/
void RenderBoard(int _board[][MAX_BOARD_POS], SDL_Surface **_array_of_numbers, int _board_pos, int _board_size_px, int _square_size_px, SDL_Renderer *_renderer )
{
    int x_corner, y_corner;
    SDL_Rect boardPos;
    SDL_Texture *number_text;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px - 15);

    // renders the squares where the numbers will appear
    for ( int i = 0; i < _board_pos; i++ )
    {
        for ( int j = 0; j < _board_pos; j++ )
        {
            // basic check
            if ( _board[i][j] > 23 || _board[i][j] < 0 )
            {
                printf("Invalid board value: RenderBoard\n");
                exit(EXIT_FAILURE);
            }
            // only draws a number if it corresponds to a non-empty space
            if ( _board[i][j] != 0 )
            {
                // define the size and copy the image to display
                boardPos.x = x_corner + (i+1)*SQUARE_SEPARATOR + i*_square_size_px;
                boardPos.y = y_corner + (j+1)*SQUARE_SEPARATOR + j*_square_size_px;
                boardPos.w = _square_size_px;
                boardPos.h = _square_size_px;
                number_text = SDL_CreateTextureFromSurface( _renderer, _array_of_numbers[ _board[i][j] - 1] );
                SDL_RenderCopy( _renderer, number_text, NULL, &boardPos );
            }
        }
    }
    // destroy texture
  //  SDL_DestroyTexture(number_text);
}

/*
* Shows some information about the game:
* - Level to achieve (difficulty)
* - Score of the game
* - Time spent in seconds
* \param _renderer renderer to handle all rendering in a window
* \param _level current level
* \param _score actual score
* \param _time time to display in seconds
*/
void RenderStats( SDL_Renderer *_renderer, TTF_Font *_font1, TTF_Font *_font2, int *difficulty, int *add, char username[STRING_SIZE],time_t timer)
{
  char level[10] = {0}, tempo[10] = {0}, score[10] = {0};
  _font1 = TTF_OpenFont("FreeSerif.ttf", 25);
  _font2 = TTF_OpenFont("FreeSerif.ttf", 40);

  SDL_Color cor = {217,217,25}; //cor das letras, amarelo
  SDL_Color cor1 = {218,165,32}; //cor das letras, dourado
  SDL_Color cor2 = { 184, 134, 11 }; //cor das instruções, castanho
  SDL_Color cor3 = { 139, 69 , 19 }; //cor dos retangulos, castanho

  SDL_SetRenderDrawColor(_renderer, cor3.r, cor3.g, cor3.b, cor3.a );

  SDL_Rect retangulo1; //linha "retangular" castanha vertical com espessura reduzida
  retangulo1.x=340;
  retangulo1.y=100;
  retangulo1.w=5;
  retangulo1.h=80;

  SDL_RenderFillRect(_renderer, &retangulo1);

  SDL_Rect retangulo2; //linha "retangular" castanha horizantal com espessura reduzida
  retangulo2.x=100;
  retangulo2.y=100;
  retangulo2.w=450;
  retangulo2.h=5;

  SDL_RenderFillRect(_renderer, &retangulo2);

  int nivel = pow(2, *difficulty);//dificuldade do jogo traduzida para o valor real. ex: difficulty = 3 , nivel = 2³ = 8
  sprintf(level, "%d", nivel);
  //dificuldade
  RenderText(125, 20, "Level", _font1, &cor, _renderer);
  RenderText(125, 40, level, _font2, &cor1, _renderer);

  // Nome do utilizador
  RenderText(360, 100, "Name", _font1, &cor, _renderer);
  RenderText(360, 120, username, _font2, &cor1, _renderer);

  //Pontuação
  sprintf(score, "%d", *add);
  RenderText(315, 20, "Score", _font1, &cor, _renderer);
  RenderText(315, 40, score, _font2, &cor1, _renderer);

  //tempo do jogo
  sprintf(tempo, "%ld", timer);
  RenderText(465, 20, "Time(s)", _font1, &cor, _renderer);
  RenderText(465, 40, tempo, _font2, &cor1, _renderer);

  //instruções
  RenderText(100, 100, "'n' --- start & new game", _font1, &cor2, _renderer);
  RenderText(100, 120, "'u' --- undo", _font1, &cor2, _renderer);
  RenderText(100, 140, "'q' --- quit game", _font1, &cor2, _renderer);

  TTF_CloseFont(_font1);
  TTF_CloseFont(_font2);


}

/*
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos number of squares in the board
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderTable( int _board_pos, int *_board_size_px, int *_square_size_px, TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer )
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color gold = {200,150,100};//{207, 181, 59}; cor do blocos do tabuleiro
    SDL_Color dark = { 120, 110, 102 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    int height, board_size_px, square_size_px;

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;
    tableDest.w = TABLE_SIZE;
    tableDest.h = TABLE_SIZE;

    // draws the table texture
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(TABLE_SIZE, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(TABLE_SIZE+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(TABLE_SIZE+3*MARGIN, height, myNumber, _font, &black, _renderer);

    // compute and adjust the size of the table and squares
    board_size_px = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size_px - (_board_pos+1)*SQUARE_SEPARATOR) / _board_pos;
    board_size_px -= board_size_px % (_board_pos*(square_size_px+SQUARE_SEPARATOR));
    board_size_px += SQUARE_SEPARATOR;

    // renders the entire board background
    SDL_SetRenderDrawColor(_renderer, dark.r, dark.g, dark.b, dark.a );
    board.x = (TABLE_SIZE - board_size_px) >> 1;
    board.y = (TABLE_SIZE - board_size_px - 15);
    board.w = board_size_px;
    board.h = board_size_px;
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear
    SDL_SetRenderDrawColor(_renderer, gold.r, gold.g, gold.b, gold.a );

    // iterate over all squares
    for ( int i = 0; i < _board_pos; i++ )
    {
        for ( int j = 0; j < _board_pos; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
    // store some variables for later use
    *_board_size_px = board_size_px;
    *_square_size_px = square_size_px;
}

/**
 * RenderLogo function: Renders the IST logo on the app window
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
	SDL_Texture *text_IST;
	SDL_Rect boardPos;

    // space occupied by the logo
	boardPos.x = x;
	boardPos.y = y;
	boardPos.w = _logoIST->w;
	boardPos.h = _logoIST->h;

    // render it
	text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
	SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
	SDL_DestroyTexture(text_IST);
	return _logoIST->h;
}

/**
 * RenderText function: Renders some text on a position inside the app window
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string with the text to be written
 * \param _font TTF font used to render the text
 * \param _color color of the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
	SDL_Surface *text_surface;
	SDL_Texture *text_texture;
	SDL_Rect solidRect;

	solidRect.x = x;
	solidRect.y = y;
    // create a surface from the string text with a predefined font
	text_surface = TTF_RenderText_Blended(_font,text,*_color);
	if(!text_surface)
	{
	    printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
	    exit(EXIT_FAILURE);
	}
    // create texture
	text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
	SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !
	SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);
    // clear memory
	SDL_DestroyTexture(text_texture);
	SDL_FreeSurface(text_surface);
	return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _font font that will be used to render the text
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer)
{
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
{
    // init SDL library
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
{
	// Init font library
	if(TTF_Init()==-1)
	{
	    printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
	}
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
	window = SDL_CreateWindow( "1024", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
    // check for error !
	if ( window == NULL )
	{
		printf("Failed to create window : %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
	renderer = SDL_CreateRenderer( _window, -1, 0 );

	if ( renderer == NULL )
	{
		printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	// set size of renderer to the same as window
	SDL_RenderSetLogicalSize( renderer, width, height );

	return renderer;
}

///////////////////////////////////////////////////////////
//////// DESENVOLVIMENTO DAS FUNÇÕES CRIADAS //////////////
///////////////////////////////////////////////////////////

//col - coluna, row - linha, max = maximo valor real da matriz no programa

void read_parameteres(char username[STRING_SIZE], char str[STRING_SIZE], int *bd_size, int *difficulty)
{
  int tamanho = 0, expoente = 0, ret = 0;

  printf("\n\nFirst of all, let's insert some initial data.\n\n");

  //introduzir o nome do ulilizador
  printf("Write your name: \n");
  fgets(username, STRING_SIZE, stdin);
  username[strlen(username) - 1] = 0;//elimina o enter, \n, do nome
  tamanho = strlen(username);//numero de caracteres da palavra
  while(tamanho > 8)
  {
    printf("Sorry, the name has to be between 1 and 8 letters.\n");
    fgets(username, STRING_SIZE, stdin);
    username[strlen(username) - 1] = 0;
    tamanho = strlen(username);
  }

  //introduzir o tamanho do tabuleiro
  do{
    printf("Insert the size, n (2 to 11), of the board (nxn): \n");
    fgets(str, STRING_SIZE, stdin);
    ret = sscanf(str, "%d", &(*bd_size));
  }while (ret != 1 || *bd_size < 2 || *bd_size > 11 );

  //introduzir a dificuldade do jogo
  do{
    printf("Insert the game level, a exponent (4 to 23) with base 2: \n");
    fgets(str, STRING_SIZE, stdin);
    ret = sscanf(str, "%d", &(expoente));
  }while (ret != 1 || expoente < 4 || expoente > 23 );

  *difficulty = expoente;
}

void clear_board(int board[][MAX_BOARD_POS], int val_board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0;//col - coluna, row - linha

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      board[col][row] = 0;//todos os elementos a matriz ficam a 0
      val_board[col][row] = 0;
    }
  }
}

void random_2 (int board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0;//col - coluna, row - linha
  int i = 0;


  srand(time(NULL));

  while (i != 2)
  {
    col = rand()% *bd_size; //random entre entre as posições do tabuleiro
    row = rand()% *bd_size; //random entre entre as posições do tabuleiro

    if (board[col][row] == 0) //só vão ser alteradas as posições vazias do tabuleiro
    {
      board[col][row] = (rand()%2) +1; // valor random entre 1 e 2 para uma posição do tabuleiro
      i++;
    }
  }
}

void random_1 (int board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0;//col - coluna, row - linha
  int i = 0;

  srand(time(NULL));

  while (i !=  1)
  {
    col = rand()% *bd_size; //random entre entre as posições do tabuleiro
    row = rand()% *bd_size; //random entre entre as posições do tabuleiro

    if (board[col][row] == 0) //só vão ser alteradas as posições vazias do tabuleiro
    {
      board[col][row] = (rand()%2) +1; // valor random entre 1 e 2 para uma posição do tabuleiro
      i++;
    }
  }
}

void right_key(int board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0, k = 0 , max = 0, aux = 0;

  max = *bd_size-1;

  for(row=0; row<=max ; row++)//linhas
    {
      for(col=max ; col>=0 ; col--)//clounas
      {
        for(k= max; k>col; k--)//coluna à direitas
        {
          if (board[k][row]==0)
          {
            aux= board[col][row];
            board[col][row]= board[k][row];
            board[k][row]=aux;
            //break;
          }
        }
      }
    }
  }

void left_key(int board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0, k = 0 , max = 0, aux = 0;

  max = *bd_size-1;

  for(row=0; row<=max ; row++)//linhas
  {
    for(col=0 ; col<= max; col++)//colunas
    {
      for(k=0 ; k<col; k++)//coluna à esquerda
      {
        if (board[k][row]==0)
        {
          aux= board[col][row];
          board[col][row]= board[k][row];
          board[k][row]=aux;
          //break;
        }
      }
    }
  }
}

void down_key(int board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0, k = 0 , max = 0, aux = 0;

  max = *bd_size-1;

  for(col=0 ; col<=max; col++)//colunas
  {
    for(row=max ;row>=0; row--)//linhas
    {
      for(k=max; k>row; k--)//linha abaixo
       {
         if (board[col][k]==0)
         {
           aux= board[col][row];
           board[col][row]= board[col][k];
           board[col][k]=aux;
           //break;
         }
       }
     }
   }
  }

void up_key(int board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0, k = 0 , max = 0, aux = 0;

  max = *bd_size-1;

  for(col=0 ; col<=max; col++)//colunas
  {
    for(row=0 ;row<=max; row++)//linhas
    {
      for(k=0; k<=row; k++)//linha acima
       {
         if (board[col][k]==0)
         {
           aux= board[col][row];
           board[col][row]= board[col][k];
           board[col][k]=aux;
           //break;
         }
       }
     }
   }
}

void up_soma(int board[][MAX_BOARD_POS], int *bd_size, int *add)
{
  int col = 0, row = 0, k = 0 , max = 0;
  max = *bd_size-1;

  for(col=0 ; col<=max; col++)//colunas
  {
    for(row=0 ;row<=max; row++)//linhas
    {
      for(k=row+1; k<=max; k++)//linha acima
      {
        if(board[col][row]!=0 && board[col][k]!=board[col][row])
        {
          k=max+1;//acaba o for para nao analisar mais valores
          //break;
        }
        else if (board[col][k] !=0 && board[col][k]==board[col][row])
        {
          board[col][row] = board[col][k]+1;
          board[col][k] =0;
          *add += pow(2,board[col][row]);
          k=max+1;//acaba o for para nao analisar mais valores
          //break;
         }
       }
     }
   }
}

void down_soma(int board[][MAX_BOARD_POS], int *bd_size, int *add)
{
  int col = 0, row = 0, k = 0 , max = 0;
  max = *bd_size-1;

  for(col=0 ; col<=max; col++)//colunas
  {
    for(row=max ;row>=0; row--)//linhas
    {
      for(k=row-1; k>=0; k--)//linha abaixo
      {
        if(board[col][row]!=0 && board[col][k]!=board[col][row])
        {
          k=(-1);
          //break;
        }
        else if (board[col][k] !=0 && board[col][k]==board[col][row])
        {
          board[col][row] = board[col][k]+1;
          board[col][k] =0;
          *add += pow(2,board[col][row]);
          k=(-1);
          //break;
        }
      }
     }
   }
}

void left_soma(int board[][MAX_BOARD_POS], int *bd_size, int *add)
{
  int col = 0, row = 0, k = 0 , max = 0;

  max = *bd_size-1;

  for(row=0; row<=max ; row++)//linhas
  {
    for(col=0 ; col<= max; col++)//colunas
    {
      for(k=col+1 ; k<=max; k++)//coluna à esquerda
      {
        if (board[k][row] !=0 && board[k][row]==board[col][row])
        {
          board[col][row] = board[k][row]+1;
          board[k][row] =0;
          *add += pow(2,board[col][row]);
          k=max+1;//acaba o for para nao analisar mais valores
          //break;
        }
        else if(board[col][row]!=0 && board[k][row]!=board[col][row])
        {
          k=max+1;//acaba o for para nao analisar mais valores
          //break;
        }
      }
    }
  }
}

void right_soma(int board[][MAX_BOARD_POS], int *bd_size, int *add)
{
  int col = 0, row = 0, k = 0 , max = 0;

  max = *bd_size-1;

  		for( row=0; row<=max ; row++)//linhas
  			{
  				for(col=max ; col>=0 ; col--)//clounas
  				{
  					for(k= col-1; k>=0; k--)//coluna à direita
  					{
              if (board[k][row] !=0 && board[k][row]==board[col][row])
              {
                board[col][row] = board[k][row]+1;
                board[k][row] =0;
                *add += pow(2,board[col][row]);
                k=(-1);//acaba o for para nao analisar mais valores
  							//break;
  						}
              else if(board[col][row]!=0 && board[k][row]!=board[col][row])
              {
                k=(-1);//acaba o for para nao analisar mais valores
                //break;
              }
  		 		  }
  				}
  		  }
}

void board_copy(int board[][MAX_BOARD_POS], int val_board[][MAX_BOARD_POS], int *bd_size, int *last_add, int *add)
{
  int col = 0, row = 0;

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      val_board[col][row] = board[col][row];//val_board é a matriz atual no tabuleiro do jogo
      *last_add = *add;//last_add é a pontuação atual do jogo
    }
  }
}

void change_board(int copy_board[][MAX_BOARD_POS], int val_board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0;//col - coluna, row - linha

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      copy_board[col][row] = val_board[col][row];//a matriz para o undo é igualada à matriz da jogada anterior
    }
  }
}
void undo_play(int board[][MAX_BOARD_POS], int copy_board[][MAX_BOARD_POS], int *bd_size, int *last_add, int *add)
{
  int col = 0, row = 0;

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
     board[col][row] = copy_board[col][row];//agora a board atual passa a ser a matriz anterior que foi guardada em copy_board
     *add = *last_add;//a pontuação atual passa a ter o valor da pontuação da jogada anterior
    }
  }
}

int check_mov(int board[][MAX_BOARD_POS], int val_board[][MAX_BOARD_POS], int *bd_size)
{
  int col = 0, row = 0;

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      if (val_board[col][row] != board[col][row])//se a matriz da jogada anterior for diferente à da jogada atual, a função toma o valor 0
      {
        return 0;
      }
    }
  }//se a matriz da jogada anterior for igual da atual, a função toma o valor 1
  return 1;
}

void victory( SDL_Renderer *_renderer, TTF_Font *_font1, int *difficulty, int *bd_size, int board[][MAX_BOARD_POS], int *start)
{
  int col = 0, row = 0;

  _font1 = TTF_OpenFont("FreeSerif.ttf", 60);

  SDL_Color win_cor = {218,165,32}; //cor amarelo dourado
  SDL_Color white = { 255, 255, 255 };//cor branca

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      if (board[col][row] == *difficulty)
      {
        SDL_SetRenderDrawColor(_renderer, win_cor.r, win_cor.g, win_cor.b, win_cor.a );
        SDL_Rect vitoria; //faixa retangular amarelo dourado
        vitoria.x=0;
        vitoria.y=225;
        vitoria.w=TABLE_SIZE;
        vitoria.h=200;
        SDL_RenderFillRect(_renderer, &vitoria);

        SDL_SetRenderDrawColor(_renderer, white.r, white.g, white.b, white.a );
        SDL_Rect branco1; //linha branca(retangulo com espessura pequena) no topo da faixa amarela
        branco1.x=0;
        branco1.y=230;
        branco1.w=TABLE_SIZE;
        branco1.h=10;
        SDL_RenderFillRect(_renderer, &branco1);

        SDL_SetRenderDrawColor(_renderer, white.r, white.g, white.b, white.a );
        SDL_Rect branco2; //linha branca(retangulo com espessura pequena) na base da faixa amarela
        branco2.x=0;
        branco2.y=410;
        branco2.w=TABLE_SIZE;
        branco2.h=10;
        SDL_RenderFillRect(_renderer, &branco2);

        RenderText(175, 275, "YOU WIN!!!", _font1, &white, _renderer);//mensagem da vitória

        *start = 0;//flag de movimento e tempo desativada

        break;
      }
     }
   }

   TTF_CloseFont(_font1);
 }

int check_defeat(int *bd_size, int board[][MAX_BOARD_POS])
{
  int col = 0, row = 0;

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      if (board[col][row] == 0)//verifica se há valores da matriz a 0
      {
        return 0;
      }
      else if( ( (col+1 < *bd_size) && (board[col][row] == board[col+1][row]) ) || ( (col-1>=0) && (board[col][row] == board[col-1][row]) ) )
      {//verifica se há valores iguais à peça que esta a ser analisada, na coluna à direita ou à esquerda
        return 0;
      }
      else if( ( (row+1 < *bd_size) && (board[col][row] == board[col][row+1]) ) || ( (row-1 >=0) && (board[col][row] == board[col][row-1]) ) )
      {//verifica se há valores iguais à peça que esta a ser analisada, na linha acima ou abaixo
        return 0;
      }
    }
  }//se a matriz tiver preenchida e se os valores à direita, à esquerda, acima e abaixo de qualquer bloco, check_mov toma o valor 1 e há derrota
  return 1;
}

void defeat( SDL_Renderer *_renderer, TTF_Font *_font1, int *start)
{
  _font1 = TTF_OpenFont("FreeSerif.ttf", 60);

  SDL_Color white = { 255, 255, 255 };
  SDL_Color lose_cor = { 139, 69 , 19 }; //cor da derrota, castanho

  SDL_SetRenderDrawColor(_renderer, lose_cor.r, lose_cor.g, lose_cor.b, lose_cor.a );
  SDL_Rect derrota; //faixa retangular castanha
  derrota.x=0;
  derrota.y=225;
  derrota.w=TABLE_SIZE;
  derrota.h=200;
  SDL_RenderFillRect(_renderer, &derrota);

  SDL_SetRenderDrawColor(_renderer, white.r, white.g, white.b, white.a );
  SDL_Rect branco1; //linha branca(retangulo com espessura pequena) no topo da faixa amarela
  branco1.x=0;
  branco1.y=230;
  branco1.w=TABLE_SIZE;
  branco1.h=10;
  SDL_RenderFillRect(_renderer, &branco1);

  SDL_SetRenderDrawColor(_renderer, white.r, white.g, white.b, white.a );
  SDL_Rect branco2; //linha branca(retangulo com espessura pequena) na base da faixa amarela
  branco2.x=0;
  branco2.y=410;
  branco2.w=TABLE_SIZE;
  branco2.h=10;
  SDL_RenderFillRect(_renderer, &branco2);

  RenderText(175, 275, "YOU LOSE! :'(", _font1, &white, _renderer);//mensagem da derrota

  *start = 0;//flag de movimento e tempo desativada

   TTF_CloseFont(_font1);
}

void statistics(char username[STRING_SIZE], int *add, time_t timer, int board[][MAX_BOARD_POS], int *bd_size, int *game)
{

    int col = 0, row = 0, a = 0, b = 0, max = 0, max1 = 0, difference = 0;
    /////////////////////////////////////////////////////////////////////////////////
    //função para descobrir o max valor do tabuleiro
    for (col = 0; col < *bd_size; col++)
    {//o for(col...) e o for(row..-) percorrem toda a matriz uma primeira vez
      for (row =0; row < *bd_size; row++)
      {
        if (board[col][row] > 0)
        {
          for (a = 0; a < *bd_size; a++)
          {//o for(a...) e o for(b...) percorrem toda a matriz uma segunda vez
            for (b = 1; b < *bd_size; b++)//o b começa em 1 para analisar logo o valor seguinte
            {
              difference = board[col][row]-board[a][b];//diferença entre cada peça para analise de qual é a peça de maior valor

              if(difference < 0)
              {
                max1 = pow(2, board[a][b]);
                if(max1 > max) max = max1;
              }
              else if(difference > 0)
              {
                max1 = pow(2, board[col][row]);
                if(max1 > max) max = max1;
              }
            }
          }
        }
      }
    }
    ///////////////////////////////////////////////////////
    ///escrita dos parametros do jogo num ficheiro .txt///
    /////////////////////////////////////////////////////
    FILE *fp;
    fp = fopen("stats.txt","a");

    fprintf(fp," ///////////////\n//Statistics//\n/////////////// \n\n");

    fprintf(fp,"Name: %s\n\n", username);

    fprintf(fp,"Game: %d\n\n", *game);
    fprintf(fp,"Max Score: %d\n", *add);
    fprintf(fp,"Game duration: %ld s\n", timer);
    fprintf(fp,"Best piece: %d \n\n", max);

    fclose(fp);
    /////////////////////////////////////////////////////////////////////////////////////////////

}

void write_save(char username[STRING_SIZE], int board[][MAX_BOARD_POS], int *game, int *bd_size, int *difficulty, int *add, time_t timer)
{
  FILE *fs;
  fs = fopen("save.txt","w");//escrita dos parametros do jogo num ficheiro .txt

  fprintf(fs,"%s\n", username);//Name
  fprintf(fs,"%d\n", *game);//number of the game
  fprintf(fs,"%d\n", *add);//Score
  fprintf(fs,"%ld\n", timer);//Game time
  fprintf(fs,"%d\n", *difficulty);//Level

  fprintf(fs,"%d\n", *bd_size);//board size
  int col = 0, row = 0;

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      fprintf(fs, "%d\n", board[col][row]);//matriz atual
    }
  }

  fclose(fs);
}

void read_save(char username[STRING_SIZE], int board[][MAX_BOARD_POS], int *game, int *bd_size, int *difficulty, int *add, int *start, long int *time_saved)
{
  FILE *fs;
  fs = fopen("save.txt","r");//leitura dos parametros do jogo num ficheiro .txt

  fscanf(fs,"%s\n", username);//Name
  fscanf(fs,"%d\n", &*game);//number of the game
  fscanf(fs,"%d\n", &(*add));//Score
  fscanf(fs,"%ld\n", &(*time_saved));//Game time
  fscanf(fs,"%d\n", &(*difficulty));//Level

  fscanf(fs,"%d\n", &(*bd_size));//board size
  int col = 0, row = 0;

  for (col = 0; col < *bd_size; col++)
  {
    for (row =0; row < *bd_size; row++)
    {
      fscanf(fs, "%d\n", &board[col][row]);//matriz guardada
    }
  }

  fclose(fs);

  *start = 1;//flag de movimento e tempo é ativada
}
